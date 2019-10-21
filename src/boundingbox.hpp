#ifndef _BOUNDINGBOX_
#define _BOUNDINGBOX_

#include <Eigen/Dense>
#include <vector>
#include <tucano/mesh.hpp>



class BoundingBox {

 //float width, height, depth;
 Eigen::Affine3f modelMatrix;
 Eigen::Vector3f min, max;
 vector<Tucano::Face> faces;

public:
	BoundingBox() {}

	BoundingBox(Eigen::Vector3f minimum, Eigen::Vector3f maximum, vector<Tucano::Face> meshFaces);

	bool intersection(Eigen::Vector3f &origin, Eigen::Vector3f &dest);

	Eigen::Affine3f getModelMatrix(void) {return modelMatrix;}

	void setModelMatrix(Eigen::Affine3f matrix) {modelMatrix = matrix;}

	static std::vector<BoundingBox> createBoundingBoxes(Tucano::Mesh objectMesh);
};

#endif