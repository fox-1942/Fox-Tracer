#ifndef RAYTRACERBOROS_FLATBVHNODE_H
#define RAYTRACERBOROS_FLATBVHNODE_H

#include <array>
#include <vector>
#include "glm/glm.hpp"
#include "BvhNode.h"

using namespace std;

class FlatBvhNode {
    glm::vec4 min;
    glm::vec4 max;
    int order;
    int isLeaf;
    int createdEmpty;
    int leftOrRight;
    array<glm::vec4, 10> indices;

public:
    FlatBvhNode();

    FlatBvhNode(glm::vec3 min, glm::vec3 max, float ind, bool isLeaf, bool createdEmpty,
                vector<glm::vec4> indices, int leftOrRight);


    static FlatBvhNode nodeConverter(const BvhNode node, int ind);

    static vector<FlatBvhNode> *putNodeIntoArray(const BvhNode * node);
};

#endif //RAYTRACERBOROS_FLATBVHNODE_H
