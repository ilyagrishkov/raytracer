#include "flyscene.hpp"
#include <GLFW/glfw3.h>




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

std::vector<face> getMesh(Tucano::Mesh mesh) {
	std::vector<face> myMesh;


	for (int i = 0; i < mesh.getNumberOfFaces(); i++) {

		Tucano::Face oldFace = mesh.getFace(i);

		Eigen::Vector3f vertex1 = mesh.getShapeModelMatrix()*((mesh.getVertex(oldFace.vertex_ids[0])).head<3>());
		Eigen::Vector3f vertex2 = mesh.getShapeModelMatrix()*(mesh.getVertex(oldFace.vertex_ids[1])).head<3>();
		Eigen::Vector3f vertex3 = mesh.getShapeModelMatrix()*(mesh.getVertex(oldFace.vertex_ids[2])).head<3>();

		Eigen::Vector3f normal = mesh.getShapeModelMatrix()*oldFace.normal;

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

std::vector<boundingBox> getBoxes(std::vector<face> mesh) {
	std::vector<boundingBox> boxes;

	boundingBox currentBox;

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
			currentBox = boundingBox();

		}
	}
	return boxes;
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

	std::vector<boundingBox> boxes = getBoxes(getMesh(mesh));

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
  std::cout << "ray tracing ..." << std::endl;

  // if no width or height passed, use dimensions of current viewport
  Eigen::Vector2i image_size(width, height);
  if (width == 0 || height == 0) {
    image_size = flycamera.getViewportSize();
  }

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

  std::vector<face> myMesh = getMesh(mesh);
  std::vector<boundingBox> boxes = getBoxes(myMesh);


#pragma omp parallel for schedule(dynamic, 1) num_threads(10)

  //Traces ray for every pixel on the screen in parallel
  for (int j = 0; j < image_size[1]; ++j) {

	  std::cout << j << std::endl;

    for (int i = 0; i < image_size[0]; ++i) {

		vectorThree myScreen_coords;

		#pragma omp critical
		{

			screen_coords = flycamera.screenToWorld(Eigen::Vector2f(i, j));
			myScreen_coords = vectorThree::toVectorThree(screen_coords);
		}

		Eigen::Vector3f temp = traceRay(myOrigin, myScreen_coords, boxes, 0);
		if (temp[1] == -1) {
			temp = NO_HIT_COLOR;
		}
		pixel_data[i][j] = temp;
		
    }
  }

  // write the ray tracing result to a PPM image
  Tucano::ImageImporter::writePPMImage("result.ppm", pixel_data);
  std::cout << "ray tracing done! " << std::endl;
}

//Checks if 
bool triangleIntersectionCheck2(vectorThree &p, vectorThree &q, const face &currentFace, vectorThree &uvw) {

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

	return true;

}

bool triangleIntersectionCheck(Eigen::Vector3f rayDirection, Eigen::Vector3f& origin, Eigen::Vector3f& dest, Eigen::Vector3f& vertex1, Eigen::Vector3f& vertex2, Eigen::Vector3f& vertex3, Eigen::Vector3f& faceNormal) {
	

	float normalRayDot = faceNormal.dot(rayDirection);

	// backface culling. this is used to stop checking the triangle if we know it's not facing the correct direction,
	// but the implementation below isn't correct. It still might be worth looking at later.
	//if(normalRayDot > 0) {

	//	std::cout << "backface culling" << std::endl;

	//	return false;
	//}

	if (fabs(normalRayDot) < 0.000001) {

		return false;
	}

	float D = faceNormal.dot(vertex1);

	float t = (faceNormal.dot(origin) + D) / normalRayDot;

	if (t < 0) {

		return false;
	}

	Eigen::Vector3f P = origin + t * rayDirection;

	Eigen::Vector3f edge1 = vertex2 - vertex1;
	Eigen::Vector3f VP1 = P - vertex1;
	Eigen::Vector3f C1 = edge1.cross(VP1);
	if (faceNormal.dot(C1) < 0) {

		return false;
	}

	Eigen::Vector3f edge2 = vertex3 - vertex2;
	Eigen::Vector3f VP2 = P - vertex2;
	Eigen::Vector3f C2 = edge2.cross(VP2);
	if (faceNormal.dot(C2) < 0) {

		return false;
	}

	Eigen::Vector3f edge3 = vertex1 - vertex3;
	Eigen::Vector3f VP3 = P - vertex3;
	Eigen::Vector3f C3 = edge3.cross(VP3);
	if (faceNormal.dot(C3) < 0) {

		return false;
	}

	return true;

}

bool boxIntersectionCheck2(vectorThree &origin, vectorThree &dest, const boundingBox &box) {

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

	if (abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) { return false; }
	if (abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) { return false; }
	if (abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) { return false; }

	return true;

}

// Traces ray
Eigen::Vector3f Flyscene::traceRay(vectorThree &origin,
                                   vectorThree &dest, std::vector<boundingBox> &boxes, int bounces, float &rayLength) {
	//Search for hit
	Triangle lightRay = traceRay(origin, dest, boxes);
	const face* hitFace = lightRay.hitFace;
	vectorThree hitPoint = lightRay.hitPoint;
	rayLength = (hitPoint - origin).length();

	//If nothing was hit, return NO_HIT_COLOR
	if (hitFace == nullptr) {
		return NO_HIT_COLOR;
	}

	//Start hard shadow
	Eigen::Vector3f color = { 0.0, 0.0, 0.0 };

	for (Eigen::Vector3f lightEigen : lights) {			//Loop over every lightsource
		vectorThree light = vectorThree::toVectorThree(lightEigen);

		Triangle shadowRay = traceRay(hitPoint, light, boxes);
		if (shadowRay.hitFace != nullptr) {				//If a face was hit, continue because it's in the shadow	
			continue;								
		}
		int matId = hitFace->material_id;				//If nothing was hit, add the material color to it
		Tucano::Material::Mtl mat = materials[matId];
		color += mat.getDiffuse();						//Add diffuse color for every lightsource
	}
	//End hard shadow

	if (bounces < MAX_BOUNCES) {
		vectorThree direction = (hitPoint - origin) / direction.length();

		vectorThree normal = hitFace->normal / normal.length();

		vectorThree refVector = direction - normal*(normal.dot(direction));

		vectorThree dest = hitPoint + refVector * 10000;

		float rayLength = 10000;

		traceRay(hitPoint, dest, boxes, bounces + 1, rayLength);

		//Do something with this reflection
	}
	
	//refVector shoots towards next possible object
	return color;
}

Triangle Flyscene::traceRay(vectorThree& origin, vectorThree& dest, std::vector<boundingBox>& boxes) {
	vectorThree uvw, point, hitPoint;
	const face* minFace = nullptr;
	float currentDistance;
	float minDistance = FLT_MAX;
	//Loops through all boxes

	vectorThree rayDirection = dest - origin;
	rayDirection.x *= 5.0;
	rayDirection.y *= 5.0;
	rayDirection.z *= 5.0;
	dest = rayDirection + origin;


	for (const boundingBox &currentBox : boxes) {
		//If ray hits a box
		if (boxIntersectionCheck2(origin, dest, currentBox)) {
			//Then it loops through all faces of that box
			for (const face& currentFace : currentBox.faces) {
				//If it hits a face in that box
				if (triangleIntersectionCheck2(origin, dest, currentFace, uvw)) {
					//This is the point it hits the triangle
					point = (currentFace.vertex1 * uvw.x) + (currentFace.vertex2 * uvw.y) + (currentFace.vertex3 * uvw.z);

					currentDistance = (point - origin).length();
					//Calculates closest triangle
					if (minDistance > currentDistance && currentDistance > 0.1) {
						minFace = &currentFace;
						hitPoint = point;
					}
				}
			}
		}
	}
	//In case ray hits nothing
	if (hitPoint == dest) {
		minFace = nullptr;
	}
	
	return { hitPoint, minFace };

}