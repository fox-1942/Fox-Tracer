//
// Created by fox1942 on 11/9/20.
//

#include <vector>
#include "../includes/glm/glm.hpp"
#include "../includes/bvhnode.h"
#include "../includes/flatbvhnode.h"


FlatBvhNode::FlatBvhNode(glm::vec3 min, glm::vec3 max, float ind, bool isLeaf, bool createdEmpty,
                         vector<glm::vec4> indices, int leftOrRight) :
        min(glm::vec4(min.x, min.y, min.z, 1.0f)),
        max(glm::vec4(max.x, max.y, max.z, 1.0f)),
        order(ind),
        isLeaf(isLeaf),
        createdEmpty(createdEmpty),
        leftOrRight(leftOrRight) {

    for (int i = 0; i < indices.size(); i++) {
        this->indices.at(i) = glm::vec4(indices.at(i).x, indices.at(i).y, indices.at(i).z, indices.at(i).w);
    }
}

FlatBvhNode FlatBvhNode::nodeConverter(BvhNode node, int ind) {
    return FlatBvhNode(node.getBBox().getMin(), node.getBBox().getMax(), ind, node.getIsLeaf(),
                       node.isCreatedEmpty(),
                       node.getIndices(), node.getLeftOrRight());
}

vector<FlatBvhNode> *FlatBvhNode::putNodeIntoArray(BvhNode *node) {

    deque<BvhNode *> queue;
    queue.push_back(node);

    vector<FlatBvhNode> *nodesArray = new vector<FlatBvhNode>;


    int ind = 0;
    while (!queue.empty()) {
        BvhNode * curr=new BvhNode(*queue.front());
        queue.pop_front();

        nodesArray->push_back(FlatBvhNode::nodeConverter(*curr, ind));

        if (!curr->getChildren().empty()) {

            queue.push_back(curr->getChildren().at(0));
            queue.push_back(curr->getChildren().at(1));
        }

        ind++;
    }
    cout << "Flatenning the tree is done." << endl;

    return nodesArray;
}