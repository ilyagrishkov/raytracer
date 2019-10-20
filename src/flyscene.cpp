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

void Flyscene::createDebugRay(const Eigen::Vector2f &mouse_pos) {
  ray.resetModelMatrix();
  // from pixel position to world coordinates
  Eigen::Vector3f screen_pos = flycamera.screenToWorld(mouse_pos);

  // direction from camera center to click position
  Eigen::Vector3f dir = (screen_pos - flycamera.getCenter()).normalized();
  
  // position and orient the cylinder representing the ray
  ray.setOriginOrientation(flycamera.getCenter(), dir);

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



 //for every pixel shoot a ray from the origin through the pixel coords

  std::vector<boundingBox> boxes;

  boundingBox currentBox;

  int faceNum = 100;

  for (int i = 0; i < mesh.getNumberOfFaces(); i++) {

	  Tucano::Face face = mesh.getFace(i);

	  Eigen::Vector3f vertex1 = (mesh.getVertex(face.vertex_ids[0])).head<3>();
	  Eigen::Vector3f vertex2 = (mesh.getVertex(face.vertex_ids[1])).head<3>();
	  Eigen::Vector3f vertex3 = (mesh.getVertex(face.vertex_ids[2])).head<3>();

	  currentBox.xMax = std::max(currentBox.xMax, vertex1[0]);
	  currentBox.xMax = std::max(currentBox.xMax, vertex2[0]);
	  currentBox.xMax = std::max(currentBox.xMax, vertex3[0]);

	  currentBox.xMin = std::min(currentBox.xMin, vertex1[0]);
	  currentBox.xMin = std::min(currentBox.xMin, vertex2[0]);
	  currentBox.xMin = std::min(currentBox.xMin, vertex3[0]);

	  currentBox.yMax = std::max(currentBox.yMax, vertex1[1]);
	  currentBox.yMax = std::max(currentBox.yMax, vertex2[1]);
	  currentBox.yMax = std::max(currentBox.yMax, vertex3[1]);

	  currentBox.yMin = std::min(currentBox.yMin, vertex1[1]);
	  currentBox.yMin = std::min(currentBox.yMin, vertex2[1]);
	  currentBox.yMin = std::min(currentBox.yMin, vertex3[1]);

	  currentBox.zMax = std::max(currentBox.zMax, vertex1[2]);
	  currentBox.zMax = std::max(currentBox.zMax, vertex2[2]);
	  currentBox.zMax = std::max(currentBox.zMax, vertex3[2]);

	  currentBox.zMin = std::min(currentBox.zMin, vertex1[2]);
	  currentBox.zMin = std::min(currentBox.zMin, vertex2[2]);
	  currentBox.zMin = std::min(currentBox.zMin, vertex3[2]);

	  currentBox.faces.push_back(face);

	  if (i % faceNum == faceNum-1 || i == mesh.getNumberOfFaces()-1) {

		  //std::cout << currentBox.faces.size() << "number of faces" << std::endl;

		  boxes.push_back(currentBox);
		  currentBox = boundingBox();

	  }

  }



#pragma omp parallel for schedule(dynamic, 1) num_threads(10)

  for (int j = 0; j < image_size[1]; ++j) {

	  std::cout << j << std::endl;

    for (int i = 0; i < image_size[0]; ++i) {
		
	

      // create a ray from the camera passing through the pixel (i,j)
     screen_coords = flycamera.screenToWorld(Eigen::Vector2f(i, j));
      // launch raytracing for the given ray and write result to pixel data
      pixel_data[i][j] = traceRay(origin, screen_coords, boxes);
    }
  }

  // write the ray tracing result to a PPM image
  Tucano::ImageImporter::writePPMImage("result.ppm", pixel_data);
  std::cout << "ray tracing done! " << std::endl;
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

bool boxIntersectionCheck(const boundingBox &box, vectorThree rayDirection, vectorThree ray, vectorThree origin) {

	vectorTwo intersection1;
	vectorTwo intersection2;

	if (rayDirection.z != 0) {

		float Zscalar1 = box.zMax - origin.z;
		float Zscalar2 = box.zMin - origin.z;

		Zscalar1 = Zscalar1 / rayDirection.z;
		Zscalar2 = Zscalar2 / rayDirection.z;

		vectorThree temp1 = rayDirection * Zscalar1 + origin;
		vectorThree temp2 = rayDirection * Zscalar2 + origin;

		intersection1 = { temp1.x, temp1.y };
		intersection2 = { temp2.x, temp2.y };
	}
	else {

		if (box.zMin >= origin.z || origin.z >= box.zMax) {
			return false;
		}

		intersection1 = {origin.x, origin.y};
		intersection2 = { (origin + ray).x, (origin + ray).y};
	}


	vectorTwo rayDirection2D = intersection1 - intersection2;
	rayDirection2D = rayDirection2D / rayDirection2D.length();

	if (rayDirection2D.y != 0) {

		float Yscalar1 = box.yMax - intersection2.y;
		float Yscalar2 = box.yMin - intersection2.y;

		Yscalar1 = Yscalar1 / rayDirection2D.y;
		Yscalar2 = Yscalar2 / rayDirection2D.y;

		vectorTwo intersection2D1 = rayDirection2D * Yscalar1+ intersection2;
		vectorTwo intersection2D2 = rayDirection2D * Yscalar2 + intersection2;

		float intersectionMaxX = std::max(intersection2D1.x, intersection2D2.x);
		float intersectionMinX = std::min(intersection2D1.x, intersection2D2.x);

		if (box.xMin <= intersectionMaxX && intersectionMinX <= box.xMax) {
			return true;
		}

	}
	else {

		float intersectionMaxX = std::max(intersection1.x, intersection2.x);
		float intersectionMinX = std::min(intersection1.x, intersection2.x);

		if (box.xMin <= intersectionMaxX && intersectionMinX <= box.xMax && box.yMin <= intersection1.y && intersection1.y <= box.yMax) {
			return true;
		}

	}

	return false;

}

Eigen::Vector3f Flyscene::traceRay(Eigen::Vector3f &origin,
                                   Eigen::Vector3f &dest, std::vector<boundingBox> &boxes) {

	//std::cout << dest.x << std::endl;

	Eigen::Vector3f rayDirection = dest - origin;

	vectorThree ray = { rayDirection[0], rayDirection[1], rayDirection[2] };
	vectorThree myRayDirection = ray / ray.length();
	vectorThree myOrigin = { origin[0], origin[1], origin[2] };

	for (const boundingBox &currentBox : boxes) {

		if (boxIntersectionCheck(currentBox, myRayDirection, ray, myOrigin)) {

			for (const Tucano::Face &face : currentBox.faces) {

				Eigen::Vector3f faceNormal = face.normal;

				Eigen::Vector3f vertex1 = (mesh.getVertex(face.vertex_ids[0])).head<3>();
				Eigen::Vector3f vertex2 = (mesh.getVertex(face.vertex_ids[1])).head<3>();
				Eigen::Vector3f vertex3 = (mesh.getVertex(face.vertex_ids[2])).head<3>();

				if (triangleIntersectionCheck(rayDirection, origin, dest, vertex1, vertex2, vertex2, faceNormal)) {

					return(Eigen::Vector3f(0.98, 0.78, 0.05));

				}

			}

		}

	}

	return(Eigen::Vector3f(0.0, 0.0, 0.0));
}



