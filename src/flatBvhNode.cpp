//
// Created by fox1942 on 11/9/20.
//

#include <vector>
#include "../includes/glm/glm.hpp"
#include "../includes/BvhNode.h"
#include "../includes/flatBvhNode.h"

using namespace std;

FlatBvhNode::FlatBvhNode() {}

FlatBvhNode::FlatBvhNode(glm::vec3 min, glm::vec3 max, float ind, bool isLeaf, bool createdEmpty,
                         vector<glm::vec4> indices, int leftOrRight) {
    this->min = glm::vec4(min.x, min.y, min.z, 1.0f);
    this->max = glm::vec4(max.x, max.y, max.z, 1.0f);
    this->order = ind;
    this->isLeaf = isLeaf;
    this->createdEmpty = createdEmpty;
    this->leftOrRight = leftOrRight;

    for (int i = 0; i < indices.size(); i++) {
        this->indices.at(i) = glm::vec4(indices.at(i).x, indices.at(i).y, indices.at(i).z, indices.at(i).w);
    }
}

FlatBvhNode FlatBvhNode::nodeConverter(const BvhNode node, int ind) {
    FlatBvhNode result = FlatBvhNode(node.getBBox().getMin(), node.getBBox().getMax(), ind, node.getIsLeaf(),
                                     node.isCreatedEmpty(),
                                     node.getIndices(), node.getLeftOrRight());
    return result;
}

vector<FlatBvhNode> *FlatBvhNode::putNodeIntoArray(const BvhNode *node) {

    deque<const BvhNode *> queue;
    queue.push_back(node);

    vector<FlatBvhNode> *nodesArray = new vector<FlatBvhNode>;

    int ind = 0;
    while (!queue.empty()) {
        const BvhNode *curr = queue.front();
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
