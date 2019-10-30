#include "flyscene.hpp"
#include <GLFW/glfw3.h>
#include "math.h"


//===========================================================================
//============================ Bounding Box =================================
//===========================================================================

void printNodes(BoundingBox &currentBox) {

  //std::cout << currentBox.xMin << " " <<  currentBox.xMin << " " << currentBox.yMin << " " << currentBox.yMax << " " << currentBox.zMin << " " << currentBox.zMax << " FACES: " << currentBox.getFaces().size() << std::endl;
  
  if(currentBox.children.size() == 0) {
	  Tucano::Shapes::Box box = Tucano::Shapes::Box(currentBox.getX(), currentBox.getY(), currentBox.getZ());
	  Eigen::Affine3f boxMatrix = box.getShapeModelMatrix();

	  boxMatrix.translate(currentBox.getCenter());

	  box.setModelMatrix(boxMatrix);

	  leafBoxes.push_back(box);
  
  }
  for (BoundingBox &box : currentBox.children) {
    printNodes(box);
  }

}

BoundingBox createBox(const std::vector<face>& mesh) {

  BoundingBox currentBox;

  for (int i = 0; i < mesh.size(); i++) {

    face currentFace = mesh[i];

    vectorThree vertex1 = currentFace.vertex1;
    vectorThree vertex2 = currentFace.vertex2;
    vectorThree vertex3 = currentFace.vertex3;

    currentBox.xMax = std::max(currentBox.xMax, vertex1.x);
    currentBox.xMax = std::max(currentBox.xMax, vertex2.x);
    currentBox.xMax = std::max(currentBox.xMax, vertex3.x);

    currentBox.xMin = std::min(currentBox.xMin, vertex1.x);
    currentBox.xMin = std::min(currentBox.xMin, vertex2.x);
    currentBox.xMin = std::min(currentBox.xMin, vertex3.x);

    currentBox.yMax = std::max(currentBox.yMax, vertex1.y);
    currentBox.yMax = std::max(currentBox.yMax, vertex2.y);
    currentBox.yMax = std::max(currentBox.yMax, vertex3.y);

    currentBox.yMin = std::min(currentBox.yMin, vertex1.y);
    currentBox.yMin = std::min(currentBox.yMin, vertex2.y);
    currentBox.yMin = std::min(currentBox.yMin, vertex3.y);

    currentBox.zMax = std::max(currentBox.zMax, vertex1.z);
    currentBox.zMax = std::max(currentBox.zMax, vertex2.z);
    currentBox.zMax = std::max(currentBox.zMax, vertex3.z);

    currentBox.zMin = std::min(currentBox.zMin, vertex1.z);
    currentBox.zMin = std::min(currentBox.zMin, vertex2.z);
    currentBox.zMin = std::min(currentBox.zMin, vertex3.z);

    currentBox.faces.push_back(currentFace);
  }

  //std::cout << currentBox.xMin << " " <<  currentBox.xMin << " " << currentBox.yMin << " " << currentBox.yMax << " " << currentBox.zMin << " " << currentBox.zMax << std::endl;
  return currentBox;
}

Eigen::Vector3f noHitMultiplier = { 1, 1, 1 };

bool sorterX(face i, face j) {
  return i.vertex1.x < j.vertex1.x;
}

bool sorterY(face i, face j) {
  return i.vertex1.y < j.vertex1.y;
}

bool sorterZ(face i, face j) {
  return i.vertex1.z < j.vertex1.z;
}

BoundingBox splitBox(BoundingBox& rootBox, int faceNum) {

  std::vector<face> faces = rootBox.faces;


  if (faces.size() > faceNum) {

    float x = rootBox.getX();
    float y = rootBox.getY();
    float z = rootBox.getZ();

    std::size_t const half_size = faces.size() / 2;
    std::size_t const third_size = faces.size() / 3;
    std::size_t const two_third_size = 2 * faces.size() / 3;

    if(x > y && x > z) {
      
      std::sort(faces.begin(), faces.end(), sorterX);
    } 
    else if(y > x && y > z) {

      std::sort(faces.begin(), faces.end(), sorterY);
    }
    else {

      std::sort(faces.begin(), faces.end(), sorterZ);
    }

    
    std::vector<face> split_first_left(faces.begin(), faces.begin() + third_size);
    std::vector<face> split_first_right(faces.begin() + third_size, faces.end());

    std::vector<face> split_second_left(faces.begin(), faces.begin() + half_size);
    std::vector<face> split_second_right(faces.begin() + half_size, faces.end());

    std::vector<face> split_third_left(faces.begin(), faces.begin() + two_third_size);
    std::vector<face> split_third_right(faces.begin() + two_third_size, faces.end());


    float first_cost = 1 + 1.0f/3.0f * split_first_left.size() * 2 + 2.0f/3.0f * split_first_right.size() * 2;
    float second_cost = 1 + 1.0f/2.0f * split_second_left.size() * 2 + 1.0f/2.0f * split_second_right.size() * 2;
    float third_cost = 1 + 2.0f/3.0f * split_third_left.size() * 2 + 1.0f/3.0f * split_third_right.size() * 2;

    BoundingBox lo_split;
    BoundingBox hi_split;

    if(first_cost > second_cost && first_cost > third_cost) {

      lo_split = createBox(split_first_left);
      hi_split = createBox(split_first_right);
    } 
    else if(second_cost > first_cost && second_cost > third_cost) {

      lo_split = createBox(split_second_left);
      hi_split = createBox(split_second_right);
    }
    else {

      lo_split = createBox(split_third_left);
      hi_split = createBox(split_third_right);
    }


    BoundingBox first_box = splitBox(lo_split, faceNum);
    BoundingBox second_box = splitBox(hi_split, faceNum);

    rootBox.addChild(first_box);
    rootBox.addChild(second_box);

  }

  return rootBox;
}

bool rayBoxIntersection(const BoundingBox &box, vectorThree& origin, vectorThree& dest) {

  rayBoxChecks++;
  vectorThree max = { box.xMax, box.yMax, box.zMax };
  vectorThree min = { box.xMin, box.yMin, box.zMin };

  vectorThree e = max - min;
  vectorThree d = dest - origin;
  vectorThree m = origin + dest - min - max;

  float adx = abs(d.x);
  if (abs(m.x) > e.x + adx) {
    return false;
  }

  float ady = abs(d.y);
  if (abs(m.y) > e.y + ady) {
    return false;
  }

  float adz = abs(d.z);
  if (abs(m.z) > e.z + adz) {
    return false;
  }

  adx += FLT_EPSILON;
  ady += FLT_EPSILON;
  adz += FLT_EPSILON;

  if (abs(m.y * d.z - m.z * d.y) > e.y* adz + e.z * ady) { return false; }
  if (abs(m.z * d.x - m.x * d.z) > e.x* adz + e.z * adx) { return false; }
  if (abs(m.x * d.y - m.y * d.x) > e.x* ady + e.y * adx) { return false; }
  rayBoxIntersections++;

  return true;
}

bool checkFront(vectorThree origin, vectorThree dest, vectorThree point) {
	vectorThree frontCheck = point - origin;
	vectorThree direction = dest - origin;
	
	if (!((direction.x < 0.000001 && frontCheck.x < 0.000001) || (direction.x > 0.000001 && frontCheck.x > 0.000001) || (direction.x == 0 && frontCheck.x == 0))) {
		return false;
	}
	if (!((direction.y < 0.000001 && frontCheck.y < 0.000001) || (direction.y > 0.000001 && frontCheck.y > 0.000001) || (direction.y == 0 && frontCheck.y == 0))) {
		return false;
	}
	if (!((direction.z < 0.000001 && frontCheck.z < 0.000001) || (direction.z > 0.000001 && frontCheck.z > 0.000001) || (direction.z == 0 && frontCheck.z == 0))) {
		return false;
	}

	return true;
}

bool rayTriangleIntersection(vectorThree& origin, vectorThree& dest, const face& currentFace, vectorThree& point, bool side) {

	rayTriangleChecks++;
	vectorThree uvw = { 0.0 , 0.0, 0.0 };
	vectorThree v0;
	vectorThree v1;
	vectorThree v2;
	v0 = currentFace.vertex1;
	v1 = currentFace.vertex2;
	v2 = currentFace.vertex3;

	vectorThree dir = dest - origin;
	vectorThree originTov0 = v0 - origin;
	vectorThree originTov1 = v1 - origin;
	vectorThree originTov2 = v2 - origin;

	vectorThree h = dir.cross(v2 - v0);
	float a = (v1 - v0).dot(h);

	if (a > -FLT_EPSILON && a < FLT_EPSILON) {
		return false;
	}


	uvw.x = dir.scalarTripleProduct(originTov2, originTov1);
	if (uvw.x < 0.0) { return false; }

	uvw.y = dir.scalarTripleProduct(originTov0, originTov2);
	if (uvw.y < 0.0) { return false; }

	uvw.z = dir.scalarTripleProduct(originTov1, originTov0);
	if (uvw.z < 0.0) { return false; }

	float denom = 1.0 / (uvw.x + uvw.y + uvw.z);
	uvw.x *= denom;
	uvw.y *= denom;
	uvw.z *= denom;

	point = ((v0 * uvw.x) + (v1 * uvw.y) + (v2 * uvw.z));

	

	if (!checkFront(origin, dest, point)) {
		return false;
	}
	
	rayTriangleIntersections++;
	//point = point + currentFace.normal * 0.00001;
	return true;
}

void intersectingChildren(const BoundingBox& currentBox, vectorThree& origin, vectorThree& dest, vector<face>& checkFaces) {

  if (currentBox.children.size() == 0) {

    checkFaces.insert(checkFaces.end(), currentBox.faces.begin(), currentBox.faces.end());
  }

  for (const BoundingBox& child : currentBox.children) {

    if (rayBoxIntersection(child, origin, dest)) {
      intersectingChildren(child, origin, dest, checkFaces);
    }
  }

}

std::vector<BoundingBox> createBoundingBoxes(Tucano::Mesh& mesh) {

  std::cout << "Creating bounding boxes...\r";
  std::cout.flush();

  std::vector<face> myMesh;

  for (int i = 0; i < mesh.getNumberOfFaces(); i++) {

    Tucano::Face oldFace = mesh.getFace(i);

    Eigen::Vector3f vertex1 = mesh.getShapeModelMatrix() * (mesh.getVertex(oldFace.vertex_ids[0])).head<3>();
    Eigen::Vector3f vertex2 = mesh.getShapeModelMatrix() * (mesh.getVertex(oldFace.vertex_ids[1])).head<3>();
    Eigen::Vector3f vertex3 = mesh.getShapeModelMatrix() * (mesh.getVertex(oldFace.vertex_ids[2])).head<3>();

    Eigen::Vector3f normal = oldFace.normal;

    face currentFace{
    {vertex1[0], vertex1[1], vertex1[2]},
    {vertex2[0], vertex2[1], vertex2[2]},
    {vertex3[0], vertex3[1], vertex3[2]},
    {normal[0], normal[1], normal[2]},
    oldFace.material_id };

    myMesh.push_back(currentFace);

  }

  std::vector<BoundingBox> boxes;

  BoundingBox currentBox = createBox(myMesh);
  splitBox(currentBox, SPLIT_FACTOR);

  //vectorThree sphereCenter = {1.0, 1.0, 1.0};
  //Sphere sphere(0.5, sphereCenter, 0);
  //currentBox.spheres.push_back(sphere);
  //printNodes(currentBox);
  boxes.push_back(currentBox);
  std::cout << "Creating bounding boxes... DONE" << std::endl;
  return boxes;
}

//===========================================================================
//=============================== Sphere ====================================
//===========================================================================

bool Sphere::intersection(vectorThree& origin, vectorThree& dest, vectorThree& point) {

  vectorThree dir = dest - origin;
  vectorThree ce = origin - center;

  float a = dir.dot(dir);
  float b = 2 * ce.dot(dir);
  float c = ce.dot(ce) - (radius * radius);

  float discriminant = b*b - 4*a*c;

  if(discriminant > 0) {

    float desc = sqrt(discriminant);

    float temp = (-b-desc)/(2*a);

    point = origin + dir * temp;

    return true;
  }

  return false;
}

//===========================================================================

//===========================================================================
//===================== Old acceleration structure ==========================
//===========================================================================


std::vector<face> getMesh(Tucano::Mesh mesh) {
  std::vector<face> myMesh;


  for (int i = 0; i < mesh.getNumberOfFaces(); i++) {

    Tucano::Face oldFace = mesh.getFace(i);

    Eigen::Vector3f vertex1 = (mesh.getVertex(oldFace.vertex_ids[0])).head<3>();
    Eigen::Vector3f vertex2 = (mesh.getVertex(oldFace.vertex_ids[1])).head<3>();
    Eigen::Vector3f vertex3 = (mesh.getVertex(oldFace.vertex_ids[2])).head<3>();

    Eigen::Vector3f normal = oldFace.normal;

    face currentFace{
    {vertex1[0], vertex1[1], vertex1[2]},
    {vertex2[0], vertex2[1], vertex2[2]},
    {vertex3[0], vertex3[1], vertex3[2]},
    {normal[0], normal[1], normal[2]},
    oldFace.material_id };

    myMesh.push_back(currentFace);

  }
  return myMesh;
}

std::vector<BoundingBox> getBoxes(std::vector<face> mesh) {
  std::vector<BoundingBox> boxes;

  BoundingBox currentBox;

  int faceNum = 100;

  for (int i = 0; i < mesh.size(); i++) {

    face currentFace = mesh[i];

    vectorThree vertex1 = currentFace.vertex1;
    vectorThree vertex2 = currentFace.vertex2;
    vectorThree vertex3 = currentFace.vertex3;

    currentBox.xMax = std::max(currentBox.xMax, vertex1.x);
    currentBox.xMax = std::max(currentBox.xMax, vertex2.x);
    currentBox.xMax = std::max(currentBox.xMax, vertex3.x);

    currentBox.xMin = std::min(currentBox.xMin, vertex1.x);
    currentBox.xMin = std::min(currentBox.xMin, vertex2.x);
    currentBox.xMin = std::min(currentBox.xMin, vertex3.x);

    currentBox.yMax = std::max(currentBox.yMax, vertex1.y);
    currentBox.yMax = std::max(currentBox.yMax, vertex2.y);
    currentBox.yMax = std::max(currentBox.yMax, vertex3.y);

    currentBox.yMin = std::min(currentBox.yMin, vertex1.y);
    currentBox.yMin = std::min(currentBox.yMin, vertex2.y);
    currentBox.yMin = std::min(currentBox.yMin, vertex3.y);

    currentBox.zMax = std::max(currentBox.zMax, vertex1.z);
    currentBox.zMax = std::max(currentBox.zMax, vertex2.z);
    currentBox.zMax = std::max(currentBox.zMax, vertex3.z);

    currentBox.zMin = std::min(currentBox.zMin, vertex1.z);
    currentBox.zMin = std::min(currentBox.zMin, vertex2.z);
    currentBox.zMin = std::min(currentBox.zMin, vertex3.z);

    currentBox.faces.push_back(currentFace);

    if (i % faceNum == faceNum - 1 || i == mesh.size() - 1) {

      //std::cout << currentBox.faces.size() << "number of faces" << std::endl;

      boxes.push_back(currentBox);
      currentBox = BoundingBox();

    }
  }
  return boxes;
}


//===========================================================================
//========================== Helper Functions ===============================
//===========================================================================


Eigen::Vector3f calculateColor(const Tucano::Material::Mtl& mat, const Eigen::Vector3f& lights, 
  const Tucano::Flycamera& flycamera, const face& currentFace, const vectorThree& point) {

  /*

  float shininess = mat.getDissolveFactor();

  vectorThree normal = currentFace.normal;
  normal = normal.normalize();
  vectorThree light_pos = vectorThree::toVectorThree(lights);
  vectorThree light_dir = light_pos - point;
  light_dir = light_dir.normalize();
  float diff = std::max((normal.dot(light_dir)), 0.0f);
  	
  Eigen::Vector3f ks = mat.getSpecular() * shininess;
  Eigen::Vector3f kd = mat.getDiffuse() * (1 - shininess);

  float reductionFactor = 1.0f / MAX_BOUNCES;

  kd[0] -= reductionFactor;
  kd[1] -= reductionFactor;
  kd[2] -= reductionFactor;

  //kd = kd * diff;

  return kd + ks;

  */

	float shininess = mat.getShininess();
	Eigen::Vector3f ks = mat.getSpecular();
	Eigen::Vector3f kd = mat.getDiffuse();
  

  vectorThree normal = currentFace.normal;
  normal = normal.normalize();

  vectorThree light_pos = vectorThree::toVectorThree(lights);

  vectorThree light_dir = light_pos - point;
  light_dir = light_dir.normalize();


  vectorThree oppositeLightDir {-light_dir.x, -light_dir.y, -light_dir.z};

  vectorThree reflect_light = oppositeLightDir.reflect(normal);
  reflect_light = reflect_light.normalize();


  vectorThree eye_pos = vectorThree::toVectorThree(flycamera.getCenter());

  vectorThree eye_dir = eye_pos - point;
  eye_dir = eye_dir.normalize();

  float diff = std::max((normal.dot(light_dir)), 0.0f);

  float spec_temp = std::max(eye_dir.dot(reflect_light), 0.0f);

  //std::cout << "EYE_DIR: (" << eye_dir.x << ", " << eye_dir.y << ", " << eye_dir.z  << ") NORMAL: (" << normal.x << ", " << normal.y << ", " << normal.z  << ") DOT: " << spec_temp << " SHININESS: " << shininess << " POW: " << std::pow(spec_temp, shininess) << std::endl;

  float spec = std::pow(spec_temp, shininess);

  return diff * kd + spec * ks;
}

void printProgressBar(int prog, int size) {

	float progress = float(prog) / float(size);
	int barWidth = 70;

	std::cout << "[";
	int pos = barWidth * progress;

	for (int i = 0; i < barWidth; ++i) {

		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}

	std::cout << "] " << int(progress * 100.0) << " %\r";

	std::cout.flush();
}

Eigen::Vector3f barycentric(const Eigen::Vector3f& hitPoint, const Eigen::Vector3f& pointA, const Eigen::Vector3f& pointB, const Eigen::Vector3f& pointC) {
	Eigen::Vector3f u = pointB - pointA;
	Eigen::Vector3f v = pointC - pointA;
	Eigen::Vector3f w = hitPoint - pointA;

	float d00 = u.dot(u);
	float d01 = u.dot(v);
	float d11 = v.dot(v);
	float denom = d00 * d11 - d01 * d01;
	float d20 = w.dot(u);
	float d21 = w.dot(v);

	float x = (d01 * d21 - d11 * d20) / denom;
	float y = (d01 * d20 - d00 * d21) / denom;
	float z = 1.0f - x - y;
	return Eigen::Vector3f(x, y, z);

}

//===========================================================================


void Flyscene::initialize(int width, int height) {
  // initiliaze the Phong Shading effect for the Opengl Previewer
  phong.initialize();

  // set the camera's projecti on matrix
  flycamera.setPerspectiveMatrix(60.0, width / (float)height, 0.1f, 100.0f);
  flycamera.setViewport(Eigen::Vector2f((float)width, (float)height));

  // load the OBJ file and materials
  Tucano::MeshImporter::loadObjFile(mesh, materials,
									"resources/models/dodgeColorTest.obj");


  // normalize the model (scale to unit cube and center at origin)
  mesh.normalizeModelMatrix();
  boxes = createBoundingBoxes(mesh);

  // pass all the materials to the Phong Shader
  for (int i = 0; i < materials.size(); ++i)
    phong.addMaterial(materials[i]);


  // set the color and size of the sphere to represent the light sources
  // same sphere is used for all sources
  lightrep.setColor(Eigen::Vector4f(1.0, 1.0, 0.0, 1.0));
  lightrep.setSize(0.15);

  // create a first ray-tracing light source at some random position
  lights.push_back(Eigen::Vector3f(-1.0, 1.0, 1.0));

  // scale the camera representation (frustum) for the ray debug
  camerarep.shapeMatrix()->scale(0.2);

  // create a first debug ray pointing at the center of the screen
  createDebugRay(Eigen::Vector2f(width / 2.0, height / 2.0));

  glEnable(GL_DEPTH_TEST);

  // for (int i = 0; i<mesh.getNumberOfFaces(); ++i){
  //   Tucano::Face face = mesh.getFace(i);    
  //   for (int j =0; j<face.vertex_ids.size(); ++j){
  //     std::cout<<"vid "<<j<<" "<<face.vertex_ids[j]<<std::endl;
  //     std::cout<<"vertex "<<mesh.getVertex(face.vertex_ids[j]).transpose()<<std::endl;
  //     std::cout<<"normal "<<mesh.getNormal(face.vertex_ids[j]).transpose()<<std::endl;
  //   }
  //   std::cout<<"mat id "<<face.material_id<<std::endl<<std::endl;
  //   std::cout<<"face   normal "<<face.normal.transpose() << std::endl << std::endl;
  // }



}

void Flyscene::changeObject(void)
{
	lights.clear();

	lights.push_back(Eigen::Vector3f(-1.0, 1.0, 1.0));
}

void Flyscene::shiftBgroundred(void)
{
	noHitMultiplier = Eigen::Vector3f{ 1, 0, 0 };
	glClearColor(0.9, 0.0, 0.0, 0.9);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::cout << "Current Color: Red" << endl;
}


void Flyscene::shiftBgroundblue(void)
{
	noHitMultiplier = Eigen::Vector3f{ 0, 0, 1 };
	glClearColor(0.0, 0.0, 0.9, 0.9);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::cout << "Current Color: Blue" << endl;
}


void Flyscene::shiftBgroundgreen(void)
{
	noHitMultiplier = Eigen::Vector3f{ 0, 1, 0 };
	glClearColor(0.0, 0.9, 0.0, 0.9);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::cout << "Current Color: Green" << endl;
}


void Flyscene::shiftBgroundwhite(void)
{
	noHitMultiplier = Eigen::Vector3f{ 1, 1, 1 };
	glClearColor(0.9, 0.9, 0.9, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::cout << "Current Color: White" << endl;
}


void Flyscene::shiftBgroundblack(void)
{
	noHitMultiplier = Eigen::Vector3f{ 0, 0, 0 };

	glClearColor(0.0, 0.0, 0.0, 0.9);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::cout << "Current Color: Black" << endl;
}

void Flyscene::printInformationDebug(int ray) {
	std::cout << std::endl;
	std::cout << "================================ RAY INFORMATION ================================" << std::endl;
	std::cout << "Information about ray: " << ray + 1 << std::endl;
	if (ray > debug_rays) {
		std::cout << "This ray doesn't exist" << std::endl;
		return;
	}

	std::cout << "Origin:	 " << rayInformation[ray][0].x << " " << rayInformation[ray][0].y << " " << rayInformation[ray][0].z << std::endl;
	std::cout << "Direction: " << rayInformation[ray][1].x << " " << rayInformation[ray][1].y << " " << rayInformation[ray][1].z << std::endl;
	std::cout << "Color:	 " << rayInformation[ray][2].x << " " << rayInformation[ray][2].y << " " << rayInformation[ray][2].z << " 0.0" << std::endl;
	std::cout << "Length:	 " << rayInformation[ray][3].x << std::endl;
	if (rayInformation[ray][4].x == -1.0) {
		std::cout << "Hit at:	 None " << std::endl;
	}
	else {
		std::cout << "Hit at:	 " << rayInformation[ray][4].x << " " << rayInformation[ray][4].y << " " << rayInformation[ray][4].z << std::endl;
	}
	std::cout << std::endl;
}


void Flyscene::paintGL(void) {

  // update the camera view matrix with the last mouse interactions
  flycamera.updateViewMatrix();
  Eigen::Vector4f viewport = flycamera.getViewport();

  // clear the screen and set background color
  glClearColor(0.9, 0.9, 0.9, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // position the scene light at the last ray-tracing light source
  scene_light.resetViewMatrix();
  scene_light.viewMatrix()->translate(-lights.back());

  // render the scene using OpenGL and one light source
  phong.render(mesh, flycamera, scene_light);

  // render the ray and camera representation for ray debug
  for (Tucano::Shapes::Cylinder ray : rays) {
	ray.render(flycamera, scene_light);
  }

  for (Tucano::Shapes::Box box : leafBoxes) {
	  box.render(flycamera, scene_light);
  }
  
  camerarep.render(flycamera, scene_light);

  // render ray tracing light sources as yellow spheres
  for (int i = 0; i < lights.size(); ++i) {
    lightrep.resetModelMatrix();
    lightrep.modelMatrix()->translate(lights[i]);
    lightrep.render(flycamera, scene_light);
  }
  

  // render coordinate system at lower right corner
  flycamera.renderAtCorner();
}

void Flyscene::simulate(GLFWwindow *window) {
  // Update the camera.
  // NOTE(mickvangelderen): GLFW 3.2 has a problem on ubuntu where some key
  // events are repeated: https://github.com/glfw/glfw/issues/747. Sucks.
  float dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? 0.1 : 0.0) -
             (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? 0.1 : 0.0);
  float dy = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS
                  ? 0.1
                  : 0.0) -
             (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
                  ? 0.1
                  : 0.0);
  float dz = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ? 0.1 : 0.0) -
             (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? 0.1 : 0.0);
  flycamera.translate(dx, dy, dz);
}

void Flyscene::createDebugRay(const Eigen::Vector2f& mouse_pos) {
	// from pixel position to world coordinates
	rays.clear();
	rayInformation.clear();

	Eigen::Vector3f screen_pos = flycamera.screenToWorld(mouse_pos);

	vectorThree myOrigin = vectorThree::toVectorThree(flycamera.getCenter());
	vectorThree myDestination = vectorThree::toVectorThree(screen_pos);
	
	traceDebugRay(myOrigin, myDestination, boxes, 0);
	
	camerarep.resetModelMatrix();
	camerarep.setModelMatrix(flycamera.getViewMatrix().inverse());
}

void Flyscene::traceDebugRay(vectorThree& origin, vectorThree& dest, std::vector<BoundingBox>& boxes, int bounces) {	
	if (bounces >= MAX_BOUNCES) {
		return;
	}

	//Make new debugray and calculate direction
	Tucano::Shapes::Cylinder debugRay = Tucano::Shapes::Cylinder(0.1, 0.0);
	Eigen::Vector3f reflectColor = { 0.0, 0.0, 0.0 };
	vectorThree dir = (dest - origin).normalize();
	debugRay.resetModelMatrix();

	//Trace where new ray hits a point and set origin and direction
	Triangle tracedRay = traceRay(origin, dest, boxes);
	debugRay.setOriginOrientation(origin.toEigenThree(), dir.toEigenThree());

	//Store origin and destination
	std::vector<vectorThree> next;
	rayInformation.push_back(next);
	rayInformation[bounces].push_back(origin);
	rayInformation[bounces].push_back(dir);

	//Determine length and color depending on if it hit something
	float rayLength;
	if (tracedRay.hitFace.empty()) {
		rayLength = RAYLENGTH;
		reflectColor = NO_HIT_COLOR.cwiseProduct(noHitMultiplier);
	}
	else {
		reflectColor = traceRay(origin, dest, boxes, bounces);
		rayLength = (tracedRay.hitPoint - origin).length();
	}
	
	//Set size and color of ray
	Eigen::Vector4f colorRay = { reflectColor[0], reflectColor[1], reflectColor[2], 0.0 };
	debugRay.setSize(0.01, rayLength);
	debugRay.setColor(colorRay);
	
	//Store color and length
	rayInformation[bounces].push_back(vectorThree::toVectorThree(reflectColor));
	rayInformation[bounces].push_back({ rayLength, 0.0, 0.0 });

	//Render new ray
	rays.push_back(debugRay);
	
	//If it doesn't hit anything, set information and return
	if (tracedRay.hitFace.empty()) {
		rayInformation[bounces].push_back({ -1.0, -1.0, -1.0 });
		debug_rays = bounces;
		return;
	}

	//If it did, store hitpoint and calculate new direction
	rayInformation[bounces].push_back(tracedRay.hitPoint);
	vectorThree reflect = calcReflection(tracedRay.hitPoint, origin, tracedRay.hitFace);

	//Make next debugray
	traceDebugRay(tracedRay.hitPoint, reflect, boxes, bounces + 1);
}

void Flyscene::raytraceScene(int width, int height) {
  auto t1 = std::chrono::high_resolution_clock::now();
  std::cout << "Ray tracing..." << std::endl;

  // if no width or height passed, use dimensions of current viewport
  Eigen::Vector2i image_size(width, height);
  if (width == 0 || height == 0) {
    image_size = flycamera.getViewportSize();
  }

  Eigen::Vector4f tempViewPort = flycamera.getViewport();

  vectorFour viewport = {tempViewPort[0] , tempViewPort[1] , tempViewPort[2] , tempViewPort[3]};

  Eigen::Matrix4f matrix = flycamera.getViewMatrix().inverse().matrix();
  vectorFour row1 = {matrix(0, 0), matrix(0, 1), matrix(0, 2), matrix(0, 3) };
  vectorFour row2 = { matrix(1, 0), matrix(1, 1), matrix(1, 2), matrix(1, 3) };
  vectorFour row3 = { matrix(2, 0), matrix(2, 1), matrix(2, 2), matrix(2, 3) };
  vectorFour row4 = { matrix(3, 0), matrix(3, 1), matrix(3, 2), matrix(3, 3) };

  // create 2d vector to hold pixel colors and resize to match image size
  vector<vector<Eigen::Vector3f>> pixel_data;
  pixel_data.resize(image_size[1]);
  for (int i = 0; i < image_size[1]; ++i)
    pixel_data[i].resize(image_size[0]);

  // origin of the ray is always the camera center
  Eigen::Vector3f origin = flycamera.getCenter();
  Eigen::Vector3f screen_coords;

  vectorThree myOrigin = vectorThree::toVectorThree(origin);

 //for every pixel shoot a ray from the origin through the pixel coords
#define N 10
#pragma omp parallel 
#pragma omp for

  // DO NOT PUT ANYTHING BETWEEN THESE TWO LINES. PLEASE.

  for (int j = 0; j < image_size[1]; ++j) {

#pragma omp critical  
	  {
	  load_progress++;
	  printProgressBar(load_progress, image_size[1]);
	  }
	  
		
	for (int i = 0; i < image_size[0]; ++i) {

		vectorThree myScreen_coords;

		Eigen::Vector3f coords = flycamera.screenToWorld(Eigen::Vector2f(i, j));
		myScreen_coords.x = coords[0];
		myScreen_coords.y = coords[1];
		myScreen_coords.z = coords[2];

		pixel_data[i][j] = traceRay(myOrigin, myScreen_coords, boxes, 0);
		
    }
  }
  std::cout << std::endl;
  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout << "=========== STATISTICS ===========" << std::endl;
  std::cout << "Resolution: " << image_size[0] << "x" << image_size[1] << std::endl;
  std::cout << "Number of ray reflections: " << MAX_BOUNCES << std::endl;
  std::cout << "Soft shadow precision: " << SOFT_SHADOW_PRECISION << std::endl;
  std::cout << "Faces per bounding box: " << SPLIT_FACTOR << std::endl;
  std::cout << "----------------------------------" << std::endl;
  std::cout << "Ray-triangle checks: " << rayTriangleChecks << std::endl;
  std::cout << "Ray-triangle intersections: " << rayTriangleIntersections << std::endl;
  std::cout << "Ray-triangle efficiency: " << round(float(rayTriangleIntersections)/float(rayTriangleChecks) * 100) << " %" << std::endl;
  std::cout << "Ray-box checks: " << rayBoxChecks << std::endl;
  std::cout << "Ray-box intersections: " << rayBoxIntersections << std::endl;
  std::cout << "Ray-box efficiency: " << round(float(rayBoxIntersections)/float(rayBoxChecks) * 100) << " %" << std::endl;
  std::cout << "----------------------------------" << std::endl;
  std::cout << "Total checks: " << rayBoxChecks + rayTriangleChecks << std::endl;
  std::cout << "Total intersections: " << rayBoxIntersections + rayTriangleIntersections << std::endl;
  std::cout << "Overall efficiency: " << round(float(rayTriangleIntersections + rayBoxIntersections)/float(rayTriangleChecks + rayBoxChecks) * 100) << " %"  << std::endl;
  std::cout << "----------------------------------" << std::endl;
  std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count()/1000.0 << " seconds" << std::endl;
  std::cout << "==================================" << std::endl;
  // write the ray tracing result to a PPM image
  Tucano::ImageImporter::writePPMImage("result.ppm", pixel_data);
  std::cout << "Ray tracing... DONE" << std::endl;
  load_progress = 0;
}


Eigen::Vector3f Flyscene::calColor(std::vector<face> hitFace, vectorThree hitPoint, std::vector<BoundingBox>& boxes, Eigen::Vector3f reflectColor) {
	Eigen::Vector3f color = { 0.0, 0.0, 0.0 };

	int matId = hitFace[0].material_id;
	Tucano::Material::Mtl mat = materials[matId];
	vectorThree shadowLight;
	vectorThree hitPointBias;
	float brightness = 0;

	

	for (Eigen::Vector3f light : lights)
	{
		shadowLight = vectorThree::toVectorThree(light);
		hitPointBias = hitPoint + (hitFace[0].normal * 0.000001);
		float radius = 0.15;

		vectorThree ray = shadowLight - hitPointBias;
		vectorThree diskNormal = { -ray.x, -ray.y, -ray.z };
		diskNormal = diskNormal.normalize();

		vectorThree a = { -diskNormal.y, diskNormal.x, diskNormal.z };
		vectorThree b = a.cross(diskNormal);

		for (int i = 0; i <= SOFT_SHADOW_PRECISION; i++) {

			float diskX = shadowLight.x + radius * cos((M_PI / (SOFT_SHADOW_PRECISION / 2)) * i) * a.x + radius * sin((M_PI / (SOFT_SHADOW_PRECISION / 2)) * i) * b.x;
			float diskY = shadowLight.y + radius * cos((M_PI / (SOFT_SHADOW_PRECISION / 2)) * i) * a.y + radius * sin((M_PI / (SOFT_SHADOW_PRECISION / 2)) * i) * b.y;
			float diskZ = shadowLight.z + radius * cos((M_PI / (SOFT_SHADOW_PRECISION / 2)) * i) * a.z + radius * sin((M_PI / (SOFT_SHADOW_PRECISION / 2)) * i) * b.z;

			vectorThree pointOndisk = { diskX, diskY, diskZ };

			Triangle sShadowRay = traceRay(hitPointBias, pointOndisk, boxes);

			if (sShadowRay.hitFace.empty() && brightness < SOFT_SHADOW_PRECISION) {
				brightness++;
			}
		}

		color += calculateColor(mat, light, flycamera, hitFace[0], hitPoint);

	}

	Eigen::Vector3f emitter = { 0.0, 0.0, 0.1 };

	color += reflectColor * mat.getDissolveFactor() + mat.getAmbient();
	color /= lights.size();

	return color * (float(brightness) / float(SOFT_SHADOW_PRECISION));
}

// Traces ray
Eigen::Vector3f Flyscene::traceRay(vectorThree &origin, vectorThree &dest, std::vector<BoundingBox> &boxes, 
									int bounces) {
	//Search for hit
	Triangle lightRay = traceRay(origin, dest, boxes);
	std::vector<face> hitFace = lightRay.hitFace;
	vectorThree hitPoint = lightRay.hitPoint;
	Eigen::Vector3f reflectColor = { 0,0,0 };

	//If nothing was hit, return NO_HIT_COLOR
	if (hitFace.empty()) {
		star++;
		//int v1 = rand() % 100;
		if ((star%100 < 50 && star%4000 > 48) || star%40000>99) {
			return NO_HIT_COLOR.cwiseProduct(noHitMultiplier);
		}
		else {
			return { 1.0, 1.0, 1.0 };
		}
	}
	
	if (bounces < MAX_BOUNCES) {
		dest = calcReflection(hitPoint, origin, hitFace);
		reflectColor = traceRay(hitPoint, dest, boxes, bounces + 1);
	}
	return calColor(hitFace, hitPoint, boxes, reflectColor);
}

vectorThree Flyscene::calcReflection(vectorThree hitPoint, vectorThree origin, std::vector<face> hitFace) {
	vectorThree direction = (hitPoint - origin).normalize();

	vectorThree normal = hitFace[0].normal.normalize();

	vectorThree refVector = (direction - normal*(normal.dot(direction)*2)).normalize();

	vectorThree dest = hitPoint + refVector * 10000;
	return dest;
	}

Triangle Flyscene::traceRay(vectorThree origin, vectorThree dest, std::vector<BoundingBox>& boxes) {
	vectorThree uvw, point, hitPoint;
	std::vector<face> minFace;
	float currentDistance;
	float minDistance = FLT_MAX;

	vectorThree origin2 = origin;
	vectorThree dest2 = dest;

  vectorThree rayDirection = dest2 - origin2;
	rayDirection.x *= 5.0;
	rayDirection.y *= 5.0;
	rayDirection.z *= 5.0;
	dest2 = rayDirection + origin2;

  
	for (const BoundingBox &currentBox : boxes) {
		//If ray hits a box
		if (rayBoxIntersection(currentBox, origin2, dest2)) {
			std::vector<face> checkFaces;
			intersectingChildren(currentBox, origin2, dest2, checkFaces);
			for (const face &currentFace : checkFaces) {
				//If it hits a face in that box	
				face oppositeFace = currentFace;
				std::swap<vectorThree>(oppositeFace.vertex2, oppositeFace.vertex3);
        
				if (rayTriangleIntersection(origin2, dest2, currentFace, point, true)) {
					//This is the point it hits the triangle
					
					currentDistance = (point - origin).length();
					//Calculates closest triangle
					if (minDistance > currentDistance && currentDistance > 0.0001) {
						minFace.resize(1);
						minDistance = currentDistance;
						minFace[0] = currentFace;
						hitPoint = point;
					}
				}
				else if (rayTriangleIntersection(origin2, dest2, oppositeFace, point, false)) {
						currentDistance = (point - origin).length();
						//Calculates closest triangle
						if (minDistance > currentDistance && currentDistance > 0.0001) {
							minFace.resize(1);
							minDistance = currentDistance;
							minFace[0] = oppositeFace;
							hitPoint = point;

					}
				}
			}
    }
      std::vector<Sphere> local_spheres = currentBox.spheres;
      for (Sphere& sphere : local_spheres) {
        //std::cout << "Sphere Check!" << local_spheres.size() << std::endl;
        face new_face;

        if(sphere.intersection(origin2, dest2, point)) {


          //std::cout << "Sphere Intersection!" << point.x << ", " << point.y << ", " << point.z << std::endl;

          currentDistance = (point - origin).length();
          if (minDistance > currentDistance && currentDistance > 0.0001) {

              minFace.resize(1);
              minDistance = currentDistance;
              new_face.normal = sphere.getNormal(point);
              new_face.material_id = sphere.getMaterialId();
              minFace[0] = new_face;
              hitPoint = point;
          }
        
      }
		}
	}
	//In case ray hits nothing
	if (hitPoint == dest2) {
		minFace.clear();
	}
  
	return { hitPoint, minFace };

}
