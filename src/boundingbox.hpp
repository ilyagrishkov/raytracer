#ifndef _BOUNDINGBOX_
#define _BOUNDINGBOX_

#include <Eigen/Dense>
#include <vector>
#include <tucano/mesh.hpp>
#include <float.h>
#include <set>

static int rayTriangleChecks = 0;
static int rayBoxChecks = 0;
static int rayTriangleIntersections = 0;
static int rayBoxIntersections = 0;


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


class BoundingBox {
public:
	std::vector<face> faces;
	std::vector<BoundingBox> children;
	float xMax;
	float xMin;
	float yMax;
	float yMin;
	float zMax;
	float zMin;


	BoundingBox(void);

	bool intersection(vectorThree &origin, vectorThree &dest);

	static std::vector<BoundingBox> createBoundingBoxes(Tucano::Mesh &mesh);

	vector<face> const &getFaces() const { return faces; }

	void addChild(BoundingBox &newchild) {children.push_back(newchild);}

	static BoundingBox createBox(const std::vector<face> &mesh);

	static BoundingBox splitBox(BoundingBox &rootBox, int faceNum);

	static void intersectingChildren(BoundingBox &currentBox, vectorThree &origin, vectorThree &dest, vector<face> &checkFaces);

	std::vector<BoundingBox> const &getChildren() const { return children; }

	static void printNodes(BoundingBox &currentBox);
};

#endif