#include "boundingbox.hpp"

BoundingBox::BoundingBox(void) {
  xMax = -FLT_MAX;
  xMin = FLT_MAX;
  yMax = -FLT_MAX;
  yMin = FLT_MAX;
  zMax = -FLT_MAX;
  zMin = FLT_MAX;
}

std::vector<BoundingBox> BoundingBox::createBoundingBoxes(Tucano::Mesh &mesh) {

  std::vector<face> myMesh;

  for (int i = 0; i < mesh.getNumberOfFaces(); i++) {

    Tucano::Face oldFace = mesh.getFace(i);

    Eigen::Vector3f vertex1 = (mesh.getVertex(oldFace.vertex_ids[0])).head<3>();
    Eigen::Vector3f vertex2 = (mesh.getVertex(oldFace.vertex_ids[1])).head<3>();
    Eigen::Vector3f vertex3 = (mesh.getVertex(oldFace.vertex_ids[2])).head<3>();

    Eigen::Vector3f normal = oldFace.normal;

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
  splitBox(currentBox, 10);

  boxes.push_back(currentBox);
 
  return boxes;
}

void BoundingBox::printNodes(BoundingBox &currentBox) {

  //std::cout << currentBox.xMin << " " <<  currentBox.xMin << " " << currentBox.yMin << " " << currentBox.yMax << " " << currentBox.zMin << " " << currentBox.zMax << " FACES: " << currentBox.getFaces().size() << std::endl;
  
  if(currentBox.children.size() == 0) {
    std::cout << currentBox.xMin << " " <<  currentBox.xMin << " " << currentBox.yMin << " " << currentBox.yMax << " " << currentBox.zMin << " " << currentBox.zMax << " FACES: " << currentBox.getFaces().size() << std::endl;
  
  }
  for (BoundingBox &box : currentBox.children) {

    std::cout << "==";
    printNodes(box);
  }

}

BoundingBox BoundingBox::createBox(const std::vector<face> &mesh) {

  BoundingBox currentBox;

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


BoundingBox BoundingBox::splitBox(BoundingBox &rootBox, int faceNum) {

  std::vector<face> faces = rootBox.getFaces();
  if(faces.size() > faceNum) {

    std::size_t const half_size = faces.size() / 2;
    
    std::vector<face> split_lo(faces.begin(), faces.begin() + half_size);
    std::vector<face> split_hi(faces.begin() + half_size, faces.end());

    BoundingBox lo_box = createBox(split_lo);
    BoundingBox hi_box = createBox(split_hi);

    BoundingBox lo_box_split = splitBox(lo_box, faceNum);
    BoundingBox hi_box_split = splitBox(hi_box, faceNum);

    rootBox.addChild(lo_box_split);
    rootBox.addChild(hi_box_split);

  }

  return rootBox;
}

bool BoundingBox::intersection(vectorThree &origin, vectorThree &dest, int &counter1, int &counter2) { 

  counter1++;
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

  counter2++;
  return true;
}

void BoundingBox::intersectingChildren(BoundingBox &currentBox, vectorThree &origin, vectorThree &dest, vector<face> &checkFaces, int &counter1, int &counter2) {

  for(BoundingBox &child : currentBox.children) {

    if(child.intersection(origin, dest, counter1, counter2)) {
      BoundingBox::intersectingChildren(child, origin, dest, checkFaces, counter1, counter2);
    }
  }

  if(currentBox.children.size() == 0) {

    checkFaces.insert(checkFaces.end(), currentBox.faces.begin(), currentBox.faces.end());
  }
}
