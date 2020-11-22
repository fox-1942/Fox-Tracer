//
// Created by fox1942 on 11/9/20.
//

#include <iostream>
#include <vector>
#include <deque>

#include "../includes/bbox.h"
#include "../includes/glm/glm.hpp"
#include "../includes/bvhnode.h"

using namespace std;

int numberOf = 1;
int numberOfLeaves = 0;
int indOrder = 0;

BvhNode::BvhNode()=default;

BvhNode::~BvhNode(){}

void BvhNode::buildTree(vector<glm::vec4> &indices, int depth) {

    if (indices.size() <= numberOfPolygonsInModel / 3) {
        if (indices.size() > numberOfPolyInTheLeafWithLargestNumberOfPoly) {
            numberOfPolyInTheLeafWithLargestNumberOfPoly = indices.size();
        }
        cout << "numberOfPolyInTheLeafWithLargestNumberOfPoly: " << numberOfPolyInTheLeafWithLargestNumberOfPoly
             << endl;

        this->bBox = BBox();
        this->indices = indices;
        this->depthOfNode = depth;
        this->isLeaf = true;
        this->order = indOrder;
        this->createdEmpty = false;
        indOrder++;
        return;
    }

    this->depthOfNode = depth;
    this->bBox = bBox.getBBox(indices);

    this->order = indOrder;
    indOrder++;

    int axis = this->bBox.getLongestAxis();

    vector<glm::vec4> leftTree;
    vector<glm::vec4> rightTree;

    for (int i = 0; i < indices.size(); ++i) {
        switch (axis) {
            case 0:
                if (bBox.getCenter().x >= this->bBox.getFaceCenters().at(i).x) {
                    leftTree.push_back(indices.at(i));
                } else if (bBox.getCenter().x < this->bBox.getFaceCenters().at(i).x) {
                    rightTree.push_back(indices.at(i));
                }
                break;
            case 1:
                if (bBox.getCenter().y >= this->bBox.getFaceCenters().at(i).y) {
                    leftTree.push_back(indices.at(i));
                } else if (bBox.getCenter().y < this->bBox.getFaceCenters().at(i).y) {
                    rightTree.push_back(indices.at(i));
                }
                break;
            case 2:
                if (bBox.getCenter().z >= this->bBox.getFaceCenters().at(i).z) {
                    leftTree.push_back(indices.at(i));
                } else if (bBox.getCenter().z < this->bBox.getFaceCenters().at(i).z) {
                    rightTree.push_back(indices.at(i));
                }
        };
    }

    if (leftTree.size() == indices.size() || rightTree.size() == indices.size()) {
        this->indices = indices;
        this->isLeaf = true;
        if (indices.size() > numberOfPolyInTheLeafWithLargestNumberOfPoly) {
            numberOfPolyInTheLeafWithLargestNumberOfPoly = indices.size();
        }

        cout << "numberOfPolyInTheLeafWithLargestNumberOfPoly: " << numberOfPolyInTheLeafWithLargestNumberOfPoly
             << endl;

        return;
    }

    cout << "Left: " << leftTree.size() << endl;
    cout << "Right: " << rightTree.size() << endl;

    BvhNode *left = new BvhNode();
    BvhNode *right = new BvhNode();

    left->buildTree(leftTree, this->depthOfNode + 1);
    right->buildTree(rightTree, this->depthOfNode + 1);

    left->leftOrRight = 0;
    children.push_back(left);

    right->leftOrRight = 1;
    children.push_back(right);


    return;
}

int BvhNode::countNodes() {
    for (int i = 0; i < this->children.size(); i++) {
        numberOf++;
        children.at(i)->countNodes();
    }
    return numberOf;
}

int BvhNode::getNumberOfLeaves() {
    for (int i = 0; i < this->children.size(); i++) {
        if (children.at(i)->isLeaf) {
            numberOfLeaves++;
        }
        children.at(i)->getNumberOfLeaves();
    }
    return numberOfLeaves;
}

int BvhNode::findDeep(int &deepest) {
    for (int i = 0; i < this->children.size(); i++) {
        if (this->children.at(i)->depthOfNode > deepest) {
            deepest = this->children.at(i)->depthOfNode;
        }
        children.at(i)->findDeep(deepest);
    }
    return deepest;
}

int BvhNode::getDeepestLevel() {
    int deepest = -1;
    return findDeep(deepest);
}

void BvhNode::treeComplete(int deepestLev) {
    if (this->children.empty() && this->depthOfNode != deepestLev) {
        this->isLeaf = true; // Commented, because semantically 'this' would remain a leaf;

        BvhNode* emptyNode=new BvhNode();
        emptyNode->bBox = BBox();
        emptyNode->createdEmpty = true;
        emptyNode->isLeaf = true;
        emptyNode->order = -1;
        emptyNode->depthOfNode = this->depthOfNode + 1;
        emptyNode->children.clear();
        emptyNode->leftOrRight = 0;
        this->children.push_back(emptyNode);


        BvhNode* emptyNode2=new BvhNode();
        *emptyNode2 = *emptyNode;
        emptyNode2->leftOrRight = 1;
        this->children.push_back(emptyNode2);
    }

    for (int i = 0; i < this->children.size(); i++) {
        children.at(i)->treeComplete(deepestLev);
    }
}

void BvhNode::makeBvHTreeComplete() {
    int deep = this->getDeepestLevel();
    this->treeComplete(deep);
}

int BvhNode::getNumberOfNodes() {
    numberOf = 1;
    return countNodes();
}

void BvhNode::InfoAboutNode() {
    numberOfLeaves = 0;

    cout << "Info about the tree:" << endl;
    cout << "------------------- " << endl;
    if (this->isLeaf) {
        cout << "Number Of leaves: " << 1 << endl;
    } else {
        cout << "Number Of leaves: " << getNumberOfLeaves() << endl;
    }
    cout << "Number Of nodes: " << getNumberOfNodes() << endl;
    cout << "Deepest level of the tree: " << getDeepestLevel() << "\n" << endl;
}

const BBox &BvhNode::getBBox() const {
    return bBox;
}

void BvhNode::setBBox(const BBox &bBox) {
    BvhNode::bBox = bBox;
}

int BvhNode::getDepthOfNode() const {
    return depthOfNode;
}

void BvhNode::setDepthOfNode(int depthOfNode) {
    BvhNode::depthOfNode = depthOfNode;
}

const vector<BvhNode *> &BvhNode::getChildren() const {
    return children;
}

void BvhNode::setChildren(const vector<BvhNode *> &children) {
    BvhNode::children = children;
}

int BvhNode::getOrder() const {
    return order;
}

void BvhNode::setOrder(int order) {
    BvhNode::order = order;
}

bool BvhNode::getIsLeaf() const {
    return isLeaf;
}

void BvhNode::setIsLeaf(bool isLeaf) {
    BvhNode::isLeaf = isLeaf;
}

bool BvhNode::isCreatedEmpty() const {
    return createdEmpty;
}

void BvhNode::setCreatedEmpty(bool createdEmpty) {
    BvhNode::createdEmpty = createdEmpty;
}

int BvhNode::getLeftOrRight() const {
    return leftOrRight;
}

void BvhNode::setLeftOrRight(int leftOrRight) {
    BvhNode::leftOrRight = leftOrRight;
}

const vector<glm::vec4> &BvhNode::getIndices() const {
    return indices;
}

void BvhNode::setIndices(const vector<glm::vec4> &indices) {
    BvhNode::indices = indices;
}

const int &BvhNode::getNumberOfPolygonsInModel() {
    return numberOfPolygonsInModel;
}

int &BvhNode::getNumberOfPolyInTheLeafWithLargestNumberOfPoly() {
    return numberOfPolyInTheLeafWithLargestNumberOfPoly;
}

void BvhNode::setNumberOfPolyInTheLeafWithLargestNumberOfPoly(int &numberOfPolyInTheLeafWithLargestNumberOfPoly) {
    BvhNode::numberOfPolyInTheLeafWithLargestNumberOfPoly = numberOfPolyInTheLeafWithLargestNumberOfPoly;
}
