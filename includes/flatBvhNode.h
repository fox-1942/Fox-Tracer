//
// Created by fox-1942 on 5/13/20.
//

#ifndef RAYTRACERBOROS_FLATBVHNODE_H
#define RAYTRACERBOROS_FLATBVHNODE_H

#include <array>
#include <vector>
#include "../Vendor/glm/glm.hpp"
#include "bvhtree.h"

using namespace std;

struct FlatBvhNode {
    glm::vec4 min;
    glm::vec4 max;
    int order;
    int isLeaf;
    int createdEmpty;
    int leftOrRight; // alignment
    array<glm::vec4, 50> indices;

    FlatBvhNode() {}

    FlatBvhNode(glm::vec3 min, glm::vec3 max, float ind, bool isLeaf, bool createdEmpty,
                vector<glm::vec4> indices, int leftOrRight) {
        this->min = glm::vec4(min.x, min.y, min.z, 1.0f);
        this->max = glm::vec4(max.x, max.y, max.z, 1.0f);
        this->order = ind;
        this->isLeaf = isLeaf;
        this->createdEmpty = createdEmpty;
        this->leftOrRight= leftOrRight;

        for (int i = 0; i < indices.size(); i++) {
            this->indices.at(i) = glm::vec4(indices.at(i).x, indices.at(i).y, indices.at(i).z, indices.at(i).w);
        }
    }
};

FlatBvhNode nodeConverter(const BvhNode node, int ind) {
    FlatBvhNode result = FlatBvhNode(node.bBox.min, node.bBox.max, ind, node.isLeaf, node.createdEmpty,
                                     node.indices, node.leftOrRight);
    return result;
}

vector<FlatBvhNode> *putNodeIntoArray(const BvhNode * node) {

    deque<const BvhNode *> queue;
    queue.push_back(node);

    vector<FlatBvhNode> *nodesArray = new vector<FlatBvhNode>;


    int ind = 0;
    while (!queue.empty()) {
        const BvhNode *curr = queue.front();
        queue.pop_front();

        nodesArray->push_back(nodeConverter(*curr, ind));

        if (!curr->children.empty()) {

            queue.push_back(curr->children.at(0));
            queue.push_back(curr->children.at(1));
        }
        ind++;
    }
    cout << "Flatenning the tree is done." << endl;

    return nodesArray;
}



/*
FlatBvhNode nodeConverterRec(BvhNode node, int& ind){
    node.createdEmpty=0;
    FlatBvhNode result = FlatBvhNode(node.bBox.min, node.bBox.max, ind, node.isLeaf, node.createdEmpty,
                                     node.indices);
    return result;
}

void flattenRecursion(const BvhNode &bvhNode, vector<FlatBvhNode>& nodes, int& ind) {
    ++ind;
    nodes.push_back(nodeConverterRec(bvhNode, ind));

    if (!bvhNode.isLeaf) {
        flattenRecursion(*bvhNode.children.at(0), nodes, ind);
        flattenRecursion(*bvhNode.children.at(1), nodes,ind);
    }
}

vector<FlatBvhNode>* flatten(const BvhNode& root) {
    vector<FlatBvhNode>* nodesArray=new vector<FlatBvhNode>;
    nodesArray->reserve(root.countNodes());

    int ind=0;
    flattenRecursion(root, *nodesArray, ind);

    return nodesArray;
}*/




#endif //RAYTRACERBOROS_FLATBVHNODE_H
