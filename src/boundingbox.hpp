#ifndef _BOUNDINGBOX_
#define _BOUNDINGBOX_

#include <Eigen/Dense>
#include <vector>
#include <tucano/mesh.hpp>
#include <float.h>

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

class BoundingBox {

public:
	std::vector<face> faces;
	std::vector<boundingBox> children;
	float xMax;
	float xMin;
	float yMax;
	float yMin;
	float zMax;
	float zMin;

	BoundingBox(void);

	bool intersection(vectorThree&, vectorThree&);

	static std::vector<BoundingBox> createBoundingBoxes(std::vector<face> &objectMesh);
};

#endif