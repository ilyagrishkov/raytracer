#include "boundingbox.hpp"

BoundingBox::BoundingBox(Eigen::Vector3f minimum, Eigen::Vector3f maximum, vector<Tucano::Face> meshFaces) {

		min = minimum;
		max = maximum;
		faces = meshFaces;

		Eigen::Matrix4f m;
		m << 	1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;

		modelMatrix.matrix() = m;
	}

bool BoundingBox::intersection(Eigen::Vector3f &origin, Eigen::Vector3f &direction) {

	float tmin = (min.x() - origin.x()) / direction.x(); 
    float tmax = (max.x() - origin.x()) / direction.x(); 
 
    if (tmin > tmax) swap(tmin, tmax); 
 
    float tymin = (min.y() - origin.y()) / direction.y(); 
    float tymax = (max.y() - origin.y()) / direction.y(); 
 
    if (tymin > tymax) swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (min.z() - origin.z()) / direction.z(); 
    float tzmax = (max.z() - origin.z()) / direction.z(); 
 
    if (tzmin > tzmax) swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    return true;
}
//
std::vector<BoundingBox> BoundingBox::createBoundingBoxes(Tucano::Mesh objectMesh) {
  
  std::vector<BoundingBox> bboxes;
  std::vector<Tucano::Face> faces;
  float normScale = objectMesh.getNormalizationScale();

  Eigen::Affine3f modelMatrix = objectMesh.getModelMatrix();

  float minX = numeric_limits<float>::max();
  float minY = numeric_limits<float>::max();
  float minZ = numeric_limits<float>::max();
  float maxX = numeric_limits<float>::min();
  float maxY = numeric_limits<float>::min();
  float maxZ = numeric_limits<float>::min();


  std::cout << "Creating bounding boxes" << std::endl;
  std::cout << "-----------------------" << std::endl;

  int faceNum = objectMesh.getNumberOfFaces(); 
  std::cout << "Number of faces: " << faceNum << std::endl;
  int facesPerBox = 100; //Do proper split here

  for(int i = 0; i < faceNum; i++) {

  	Tucano::Face face = objectMesh.getFace(i);

	Eigen::Vector3f vertex1 = (objectMesh.getVertex(face.vertex_ids[0])).head<3>();
	Eigen::Vector3f vertex2 = (objectMesh.getVertex(face.vertex_ids[1])).head<3>();
	Eigen::Vector3f vertex3 = (objectMesh.getVertex(face.vertex_ids[2])).head<3>();


	maxX = std::max(maxX, vertex1[0]);
	maxX = std::max(maxX, vertex2[0]);
	maxX = std::max(maxX, vertex3[0]);

	minX = std::min(minX, vertex1[0]);
	minX = std::min(minX, vertex2[0]);
	minX = std::min(minX, vertex3[0]);

	maxY = std::max(maxY, vertex1[1]);
	maxY = std::max(maxY, vertex2[1]);
	maxY = std::max(maxY, vertex3[1]);

	minY = std::min(minY, vertex1[1]);
	minY = std::min(minY, vertex2[1]);
	minY = std::min(minY, vertex3[1]);

	maxZ = std::max(maxZ, vertex1[2]);
	maxZ = std::max(maxZ, vertex2[2]);
	maxZ = std::max(maxZ, vertex3[2]);

	minZ = std::min(minZ, vertex1[2]);
	minZ = std::min(minZ, vertex2[2]);
	minZ = std::min(minZ, vertex3[2]);

	faces.push_back(face);

	if(i % facesPerBox == facesPerBox-1 || i == faceNum-1) {


		BoundingBox bbox (Eigen::Vector3f(minX, minY, minZ), Eigen::Vector3f(maxX, maxY, maxZ), faces);

  		bboxes.push_back(bbox);

  		faces = {};

		float minX = numeric_limits<float>::max();
  		float minY = numeric_limits<float>::max();
  		float minZ = numeric_limits<float>::max();
  		float maxX = numeric_limits<float>::min();
  		float maxY = numeric_limits<float>::min();
  		float maxZ = numeric_limits<float>::min();
	}

  }

	std::cout << "BBOXES SIZE: " << bboxes.size() << std::endl;
  	std::cout << "-----------------------" << std::endl;

  	return bboxes;
}
