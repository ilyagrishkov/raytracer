#ifndef __FLYSCENE__
#define __FLYSCENE__

// Must be included before glfw.
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <tucano/effects/phongmaterialshader.hpp>
#include <tucano/mesh.hpp>
#include <tucano/shapes/camerarep.hpp>
#include <tucano/shapes/cylinder.hpp>
#include <tucano/shapes/sphere.hpp>
#include <tucano/utils/flycamera.hpp>
#include <tucano/utils/imageIO.hpp>
#include <tucano/utils/mtlIO.hpp>
#include <tucano/utils/objimporter.hpp>
#include <float.h>


static float RAYLENGTH = 10.0;
static const int MAX_DEPTH = 5;


struct vectorTwo {
	float x;
	float y;

	vectorTwo operator/ (float divisor) {
		vectorTwo out;
		out.x = x / divisor;
		out.y = y / divisor;
		return out;
	}

	vectorTwo operator- (vectorTwo other) {
		vectorTwo out;
		out.x = x - other.x;
		out.y = y - other.y;
		return out;
	}

	vectorTwo operator+ (vectorTwo other) {
		vectorTwo out;
		out.x = x + other.x;
		out.y = y + other.y;
		return out;
	}

	vectorTwo operator* (vectorTwo other) {
		vectorTwo out;
		out.x = other.x * x;
		out.y = other.y * y;
		return out;
	}

	vectorTwo operator* (float other) {
		vectorTwo out;
		out.x = other * x;
		out.y = other * y;
		return out;
	}

	float length() {
		return sqrt(x * x + y * y);
	}
};

struct vectorThree {
	float x;
	float y;
	float z;

	vectorThree operator/ (float divisor) {
		vectorThree out;
		out.x = x / divisor;
		out.y = y / divisor;
		out.z = z / divisor;
		return out;
	}

	vectorThree operator- (vectorThree other) {
		vectorThree out;
		out.x = x - other.x;
		out.y = y - other.y;
		out.z = z - other.z;
		return out;
	}

	vectorThree operator+ (vectorThree other) {
		vectorThree out;
		out.x = x + other.x;
		out.y = y + other.y;
		out.z = z + other.z;
		return out;
	}

	vectorThree operator* (vectorThree other) {
		vectorThree out;
		out.x = other.x * x;
		out.y = other.y * y;
		out.z = other.z * z;
		return out;
	}

	vectorThree operator* (float other) {
		vectorThree out;
		out.x = other * x;
		out.y = other * y;
		out.z = other * z;
		return out;
	}

	vectorThree operator* (float other) const {
		vectorThree out;
		out.x = other * x;
		out.y = other * y;
		out.z = other * z;
		return out;
	}

	bool operator== (vectorThree other) {
		if (x == other.x && y == other.y && z == other.z) {
			return true;
		}
		return false;
	}

	

	float dot(vectorThree other) {
		float result = 0;
		result += x * other.x;
		result += y * other.y;
		result += z * other.z;
		return result;
	}

	vectorThree cross(vectorThree other) {
		vectorThree result;
		result.x = (y * other.z) - (z * other.y);
		result.y = (z * other.x) - (x * other.z);
		result.z = (x * other.y) - (y * other.x);
		return result;
	}

	float scalarTripleProduct(vectorThree v, vectorThree w) {
		return cross(v).dot(w);
	}

	float length() {
		return sqrt(x * x + y * y + z * z);
	}

	static vectorThree toVectorThree(Eigen::Vector3f old) {
		vectorThree out = { old(0), old(1), old(2) };
		return out;
	}
};

struct face {
	vectorThree vertex1;
	vectorThree vertex2;
	vectorThree vertex3;
	vectorThree normal;
	int material_id;
};

struct boundingBox {
	float xMax;
	float xMin;
	float yMax;
	float yMin;
	float zMax;
	float zMin;
	std::vector<boundingBox> children;
	std::vector<face> faces;

	boundingBox() {
		xMax = -FLT_MAX;
		xMin = FLT_MAX;
		yMax = -FLT_MAX;
		yMin = FLT_MAX;
		zMax = -FLT_MAX;
		zMin = FLT_MAX;
	}
};

class Flyscene {

public:
  Flyscene(void) {}

  /**
   * @brief Initializes the shader effect
   * @param width Window width in pixels
   * @param height Window height in pixels
   */
  void initialize(int width, int height);

  /**
   * Repaints screen buffer.
   **/
  virtual void paintGL();

  /**
   * Perform a single simulation step.
   **/
  virtual void simulate(GLFWwindow *window);

  /**
   * Returns the pointer to the flycamera instance
   * @return pointer to flycamera
   **/
  Tucano::Flycamera *getCamera(void) { return &flycamera; }

  /**
   * @brief Add a new light source
   */
  void addLight(void) { lights.push_back(flycamera.getCenter()); }

  /**
   * @brief Create a debug ray at the current camera location and passing
   * through pixel that mouse is over
   * @param mouse_pos Mouse cursor position in pixels
   */
  void createDebugRay(const Eigen::Vector2f &mouse_pos);

  /**
   * @brief raytrace your scene from current camera position   
   */
  void raytraceScene(int width = 0, int height = 0);

  /**
   * @brief trace a single ray from the camera passing through dest
   * @param origin Ray origin
   * @param dest Other point on the ray, usually screen coordinates
   * @return a RGB color
   */
  Eigen::Vector3f traceRay(vectorThree &origin, vectorThree &dest, std::vector<boundingBox> &boxes, float &lengthRay=RAYLENGTH);
  Eigen::Vector3f calculateHardShadow(vectorThree& origin, std::vector<boundingBox>& boxes);
private:
  // A simple phong shader for rendering meshes
  Tucano::Effects::PhongMaterial phong;

  // A fly through camera
  Tucano::Flycamera flycamera;

  // the size of the image generated by ray tracing
  Eigen::Vector2i raytracing_image_size;

  // A camera representation for animating path (false means that we do not
  // render front face)
  Tucano::Shapes::CameraRep camerarep = Tucano::Shapes::CameraRep(false);

  // a frustum to represent the camera in the scene
  Tucano::Shapes::Sphere lightrep;

  // light sources for ray tracing
  vector<Eigen::Vector3f> lights;

  // Scene light represented as a camera
  Tucano::Camera scene_light;

  /// A very thin cylinder to draw a debug ray
  Tucano::Shapes::Cylinder ray = Tucano::Shapes::Cylinder(0.1, 1.0, 16, 64);

  // Scene meshes
  Tucano::Mesh mesh;

  /// MTL materials
  vector<Tucano::Material::Mtl> materials;
};

#endif // FLYSCENE
