#ifndef _BOUNDINGSPHERE_
#define _BOUNDINGSPHERE_

#include <Eigen/Dense>



class BoundingSphere {

 float radius;
 Eigen::Affine3f modelMatrix;

public:
	BoundingSphere() {}

	BoundingSphere(float r);

	Eigen::Affine3f getModelMatrix(void) {return modelMatrix;}

	void setModelMatrix(Eigen::Affine3f matrix) {modelMatrix = matrix;}

	bool intersection(Eigen::Vector3f &origin, Eigen::Vector3f &dest);
};

#endif