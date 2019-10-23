#include "boundingbox.hpp"

BoundingBox::BoundingBox(void) {
  xMax = -FLT_MAX;
  xMin = FLT_MAX;
  yMax = -FLT_MAX;
  yMin = FLT_MAX;
  zMax = -FLT_MAX;
  zMin = FLT_MAX;
}

std::vector<BoundingBox> BoundingBox::createBoundingBoxes(std::vector<face> &mesh) {

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

bool BoundingBox::intersection(vectorThree &origin, vectorThree &dest) { 

  vectorThree max = { xMax, yMax, zMax };
  vectorThree min = { xMin, yMin, zMin };

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
