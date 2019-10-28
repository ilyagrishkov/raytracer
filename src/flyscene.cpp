#include "flyscene.hpp"
#include <GLFW/glfw3.h>
#include "math.h"


//===========================================================================
//============================ Bounding Box =================================
//===========================================================================

void printNodes(BoundingBox &currentBox) {

  //std::cout << currentBox.xMin << " " <<  currentBox.xMin << " " << currentBox.yMin << " " << currentBox.yMax << " " << currentBox.zMin << " " << currentBox.zMax << " FACES: " << currentBox.getFaces().size() << std::endl;
  
  if(currentBox.children.size() == 0) {
    std::cout << "VOLUME: " << currentBox.getVolume() << " FACES: " << currentBox.faces.size() << std::endl;
  
  }
  for (BoundingBox &box : currentBox.children) {
    printNodes(box);
  }

}

BoundingBox createBox(const std::vector<face>& mesh) {

  BoundingBox currentBox;

  std::cout << "splitting box" << std::endl;

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

  //std::cout << "splitting box" << std::endl;

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

bool rayTriangleIntersection(vectorThree& p, vectorThree& q, const face& currentFace, vectorThree& uvw) {

	rayTriangleChecks++;

	vectorThree a = currentFace.vertex1;
	vectorThree b = currentFace.vertex2;
	vectorThree c = currentFace.vertex3;

	vectorThree pq = q - p;
	vectorThree pa = a - p;
	vectorThree pb = b - p;
	vectorThree pc = c - p;

	uvw.x = pq.scalarTripleProduct(pc, pb);
	if (uvw.x < 0.0) { return false; }

	uvw.y = pq.scalarTripleProduct(pa, pc);
	if (uvw.y < 0.0) { return false; }

	uvw.z = pq.scalarTripleProduct(pb, pa);
	if (uvw.z < 0.0) { return false; }

	float denom = 1.0 / (uvw.x + uvw.y + uvw.z);
	uvw.x *= denom;
	uvw.y *= denom;
	uvw.z *= denom;

	rayTriangleIntersections++;

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

  std::vector<face> myMesh;

  for (int i = 0; i < mesh.getNumberOfFaces(); i++) {

    Tucano::Face oldFace = mesh.getFace(i);

    Eigen::Vector3f vertex1 = mesh.getShapeModelMatrix() * (mesh.getVertex(oldFace.vertex_ids[0])).head<3>();
    Eigen::Vector3f vertex2 = mesh.getShapeModelMatrix() * (mesh.getVertex(oldFace.vertex_ids[1])).head<3>();
    Eigen::Vector3f vertex3 = mesh.getShapeModelMatrix() * (mesh.getVertex(oldFace.vertex_ids[2])).head<3>();

    Eigen::Vector3f normal = mesh.getShapeModelMatrix() * oldFace.normal;

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
  splitBox(currentBox, 100);

  //printNodes(currentBox);
  boxes.push_back(currentBox);

  return boxes;
}


//===========================================================================
//========================== Helper Functions ===============================
//===========================================================================


Eigen::Vector3f calculateColor(const Tucano::Material::Mtl& mat, const Eigen::Vector3f& lights, 
  const Tucano::Flycamera& flycamera, const face& currentFace, const vectorThree& point) {

  Eigen::Vector3f kd = mat.getDiffuse();
  Eigen::Vector3f ks = mat.getSpecular();
  float shininess = mat.getShininess();

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

//===========================================================================


void Flyscene::initialize(int width, int height) {
  // initiliaze the Phong Shading effect for the Opengl Previewer
  phong.initialize();

  // set the camera's projection matrix
  flycamera.setPerspectiveMatrix(60.0, width / (float)height, 0.1f, 100.0f);
  flycamera.setViewport(Eigen::Vector2f((float)width, (float)height));

  // load the OBJ file and materials
  Tucano::MeshImporter::loadObjFile(mesh, materials,
									"resources/models/dodgeColorTest.obj");


  // normalize the model (scale to unit cube and center at origin)
  mesh.normalizeModelMatrix();

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

  // the debug ray is a cylinder, set the radius and length of the cylinder
  ray.setSize(0.005, 10.0);

  // craete a first debug ray pointing at the center of the screen
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
  ray.render(flycamera, scene_light);
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
	float rayLength = RAYLENGTH;
	ray.resetModelMatrix();

	// from pixel position to world coordinates
	Eigen::Vector3f screen_pos = flycamera.screenToWorld(mouse_pos);

	// direction from camera center to click position
	Eigen::Vector3f dir = (screen_pos - flycamera.getCenter()).normalized();

	// position and orient the cylinder representing the ray
	ray.setOriginOrientation(flycamera.getCenter(), dir);

	vectorThree myOrigin = vectorThree::toVectorThree(flycamera.getCenter());
	vectorThree myDestination = vectorThree::toVectorThree(screen_pos);

	std::vector<BoundingBox> boxes = createBoundingBoxes(mesh);

	Eigen::Vector3f colorPoint = traceRay(myOrigin, myDestination, boxes, 0, rayLength);
	if (colorPoint[1] == -1.0) {
		colorPoint = NO_HIT_COLOR;
	}

	ray.setSize(ray.getRadius(), rayLength);
	ray.render(flycamera, scene_light);

	// place the camera representation (frustum) on current camera location, 
	camerarep.resetModelMatrix();
	camerarep.setModelMatrix(flycamera.getViewMatrix().inverse());

	}



void Flyscene::raytraceScene(int width, int height) {
  auto t1 = std::chrono::high_resolution_clock::now();
  std::cout << "ray tracing ..." << std::endl;

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

  std::vector<BoundingBox> boxes = createBoundingBoxes(mesh);

#pragma omp parallel for schedule(dynamic, 1) num_threads(10)

  // DO NOT PUT ANYTHING BETWEEN THESE TWO LINES. PLEASE.

  for (int j = 0; j < image_size[1]; ++j) {

	  if (j % 10 == 0) {
		  std::cout << j << std::endl;
	  }

	  // The progress bar wasn't working for the threaded version, so I removed it.
	  // I do think we should have something more elegant than the counter above, but we can work on that when the essentials are done.
		
	for (int i = 0; i < image_size[0]; ++i) {

		vectorThree myScreen_coords;

		vectorTwo v2 = { i, j };

		vectorFour norm_coords = {
		2.0 * (v2.x - viewport.x) / viewport.z - 1.0,
		1.0 - 2.0 * (v2.y - viewport.y) / viewport.w,
		-1.0 , 1.0};
     
		float scale = 1.0 / flycamera.getPerspectiveScale();
		norm_coords.x *= flycamera.getViewportAspectRatio() * scale;
		norm_coords.y *= scale;

		myScreen_coords.x = row1.dot(norm_coords);
		myScreen_coords.y = row2.dot(norm_coords);
		myScreen_coords.z = row3.dot(norm_coords);

		Eigen::Vector3f temp = traceRay(myOrigin, myScreen_coords, boxes, 0);
		if (temp[1] == -1) {
			temp = NO_HIT_COLOR;
		}
		pixel_data[i][j] = temp;
		
    }
  }
  std::cout << std::endl;
  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout << "=========== STATISTICS ===========" << std::endl;
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
  std::cout << "ray tracing done! " << std::endl;
}



// Traces ray
Eigen::Vector3f Flyscene::traceRay(vectorThree& origin,
	vectorThree& dest, std::vector<BoundingBox>& boxes, int bounces, float& rayLength) {
	//Search for hit
	Triangle lightRay = traceRay(origin, dest, boxes);
	std::vector<face> hitFace = lightRay.hitFace;
	vectorThree hitPoint = lightRay.hitPoint;
	rayLength = (hitPoint - origin).length();

	//If nothing was hit, return NO_HIT_COLOR
	if (hitFace.empty()) {
		return NO_HIT_COLOR;
	}

	if (bounces < MAX_BOUNCES) {
		vectorThree direction = (hitPoint - origin) / direction.length();

		vectorThree normal = hitFace[0].normal / normal.length();

		vectorThree refVector = direction - normal * (normal.dot(direction));

		vectorThree dest = hitPoint + refVector * 10000;

		float rayLength = 10000;

		traceRay(hitPoint, dest, boxes, bounces + 1, rayLength);

		//Do something with this reflection
	}

	Eigen::Vector3f color = { 0.0, 0.0, 0.0 };

	int matId = hitFace[0].material_id;
	Tucano::Material::Mtl mat = materials[matId];
	vectorThree shadowLight;
	vectorThree hitPointBias;
	Eigen::Vector3f shadowpoint = { 0.0, 0.0, 0.0 };
	Eigen::Vector3f _crosser = { 0, 0, 1 };
	vectorThree shadowPoint = vectorThree::toVectorThree(shadowpoint);
	vectorThree crosser = vectorThree::toVectorThree(_crosser);
	for (Eigen::Vector3f light : lights)
	{
		shadowLight = vectorThree::toVectorThree(light);
		hitPointBias = hitPoint + (hitFace[0].normal * 0.008);
		vectorThree normal = (shadowLight - hitPointBias).normalize();
		vectorThree normala;
		if (normal.z != 1) {
			normala = normal.cross(crosser).normalize();
		}
		else
		{
			crosser.x = 1;
			crosser.z = 0;
			normala = normal.cross(crosser).normalize();
			Eigen::Vector3f _crosser = { 0, 0, 1 };
		}
		vectorThree normalb = normala.cross(normal).normalize();
		//Triangle shadowRay = traceRay(hitPointBias, shadowLight, boxes);
		//if (shadowRay.hitFace.empty()) {
			color = color + calculateColor(mat, light, flycamera, hitFace[0], hitPoint);
		//}
		for (int i = 1; i < 9; i++)
			{
				float normalis = (cos((M_PI * 2) / i) * .15);
				float normaliz = (sin((M_PI * 2) / i) * .15);
				normala.x = normala.x * normalis;
				normala.y = normala.y * normalis;
				normala.z = normala.z * normalis;
				normalb.x = normalb.x * normaliz;
				normalb.y = normalb.y * normaliz;
				normalb.z = normalb.z * normaliz;

				vectorThree addshadow = normala + normalb;

				vectorThree sShadowPoint;

				sShadowPoint.x = addshadow.x + shadowLight.x;
				sShadowPoint.y = addshadow.y + shadowLight.y;
				sShadowPoint.z = addshadow.z + shadowLight.z;
				Triangle sShadowRay = traceRay(hitPointBias, sShadowPoint, boxes);
				if (!sShadowRay.hitFace.empty())
				{
					color = color - Eigen::Vector3f{1 / 8, 1 / 8, 1 / 8};
				}
		}
	}
	color = color + mat.getAmbient();
	return color;
}

Triangle Flyscene::traceRay(vectorThree& origin, vectorThree& dest, std::vector<BoundingBox>& boxes) {
	vectorThree uvw, point, hitPoint;
	std::vector<face> minFace;
	float currentDistance;
	float minDistance = FLT_MAX;
  vectorThree rayDirection = dest - origin;
	rayDirection.x *= 5.0;
	rayDirection.y *= 5.0;
	rayDirection.z *= 5.0;
	dest = rayDirection + origin;

  
	for (const BoundingBox &currentBox : boxes) {
		//If ray hits a box
		if (rayBoxIntersection(currentBox, origin, dest)) {
			std::vector<face> checkFaces;
			intersectingChildren(currentBox, origin, dest, checkFaces);
			for (const face &currentFace : checkFaces) {
				//If it hits a face in that box	
        face oppositeFace = currentFace;
				std::swap<vectorThree>(oppositeFace.vertex1, oppositeFace.vertex2);
        
				if (rayTriangleIntersection(origin, dest, currentFace, uvw)) {
					//This is the point it hits the triangle
					point = (currentFace.vertex1 * uvw.x) + (currentFace.vertex2 * uvw.y) + (currentFace.vertex3 * uvw.z);

					currentDistance = (point - origin).length();
					//Calculates closest triangle
					if (minDistance > currentDistance && currentDistance > 0.0001) {
						minFace.resize(1);
						minDistance = currentDistance;
						minFace[0] = currentFace;
						hitPoint = point;
					}
				}
				else if (rayTriangleIntersection(origin, dest, oppositeFace, uvw)) {
					point = (oppositeFace.vertex1 * uvw.x) + (oppositeFace.vertex2 * uvw.y) + (oppositeFace.vertex3 * uvw.z);

					currentDistance = (point - origin).length();
					//Calculates closest triangle
					if (minDistance > currentDistance && currentDistance > 0.0001) {
						minFace.resize(1);
						minDistance = currentDistance;
						minFace[0] = currentFace;
						hitPoint = point;

					}
				}
			}
		}
	}
	//In case ray hits nothing
	if (hitPoint == dest) {
		minFace.clear();
	}
  
	return { hitPoint, minFace };

}
