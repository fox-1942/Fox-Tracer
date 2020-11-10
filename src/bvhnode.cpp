//
// Created by fox1942 on 11/9/20.
//

#include <iostream>
#include <map>
#include <vector>
#include <deque>

#include "../includes/bbox.h"
#include "../includes/glm/glm.hpp"
#include "../includes/bvhnode.h"

using namespace std;

int maxNumberOfPolyInALeaf = 0;
int numberOf = 1;
int numberOfLeaves = 0;
int indOrder = 0;


int bvhnode::countNodes() const {
    for (int i = 0; i < this->children.size(); i++) {
        numberOf++;
        children.at(i)->countNodes();
    }
    return numberOf;
}

bvhnode::bvhnode() {}

bvhnode::~bvhnode() {}

void bvhnode::buildTree(vector<glm::vec4> &indices, int depth) {

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
    this->isLeaf = false;
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

    bvhnode *left = new bvhnode();
    bvhnode *right = new bvhnode();

    left->buildTree(leftTree, this->depthOfNode + 1);
    right->buildTree(rightTree, this->depthOfNode + 1);

    left->leftOrRight = 0;
    children.push_back(left);
    right->leftOrRight = 1;
    children.push_back(right);

    return;
}

int bvhnode::getNumberOfLeaves() {
    for (int i = 0; i < this->children.size(); i++) {
        if (children.at(i)->isLeaf) {
            numberOfLeaves++;
        }
        children.at(i)->getNumberOfLeaves();
    }
    return numberOfLeaves;
}

int bvhnode::findDeep(int &deepest) {
    for (int i = 0; i < this->children.size(); i++) {
        if (this->children.at(i)->depthOfNode > deepest) {
            deepest = this->children.at(i)->depthOfNode;
        }
        children.at(i)->findDeep(deepest);
    }
    return deepest;
}

int bvhnode::getDeepestLevel() {
    int deepest = -1;
    return findDeep(deepest);
}

void bvhnode::treeComplete(int deepestLev) {
    if (this->children.empty() && this->depthOfNode != deepestLev) {
        this->isLeaf = true; // Commented, because semantically 'this' would remain a leaf;

        bvhnode *emptyNode = new bvhnode();
        emptyNode->bBox = BBox();
        emptyNode->createdEmpty = true;
        emptyNode->isLeaf = true;
        emptyNode->order = -1;
        emptyNode->depthOfNode = this->depthOfNode + 1;
        emptyNode->children.clear();
        emptyNode->leftOrRight = 0;
        this->children.push_back(emptyNode);

        bvhnode *emptyNode2 = new bvhnode();
        *emptyNode2 = *emptyNode;
        emptyNode2->leftOrRight = 1;
        this->children.push_back(emptyNode2);
    }

    /*if (this->children.size()==1 && this->depthOfNode != deepestLev) {
        BvhNode emptyNode;
        emptyNode.createdEmpty = true;
        emptyNode.depthOfNode = this->depthOfNode + 1;
        this->children.push_back(emptyNode);
    }*/

    for (int i = 0; i < this->children.size(); i++) {
        children.at(i)->treeComplete(deepestLev);
    }
}

void bvhnode::makeBvHTreeComplete() {
    int deep = this->getDeepestLevel();
    this->treeComplete(deep);
}

int bvhnode::getNumberOfNodes() {
    numberOf = 1;
    return countNodes();
}

void bvhnode::InfoAboutNode() {
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

const BBox &bvhnode::getBBox() const {
    return bBox;
}

void bvhnode::setBBox(const BBox &bBox) {
    bvhnode::bBox = bBox;
}

int bvhnode::getDepthOfNode() const {
    return depthOfNode;
}

void bvhnode::setDepthOfNode(int depthOfNode) {
    bvhnode::depthOfNode = depthOfNode;
}

const vector<bvhnode *> &bvhnode::getChildren() const {
    return children;
}

void bvhnode::setChildren(const vector<bvhnode *> &children) {
    bvhnode::children = children;
}

int bvhnode::getOrder() const {
    return order;
}

void bvhnode::setOrder(int order) {
    bvhnode::order = order;
}

bool bvhnode::getIsLeaf() const {
    return isLeaf;
}

void bvhnode::setIsLeaf(bool isLeaf) {
    bvhnode::isLeaf = isLeaf;
}

bool bvhnode::isCreatedEmpty() const {
    return createdEmpty;
}

void bvhnode::setCreatedEmpty(bool createdEmpty) {
    bvhnode::createdEmpty = createdEmpty;
}

int bvhnode::getLeftOrRight() const {
    return leftOrRight;
}

void bvhnode::setLeftOrRight(int leftOrRight) {
    bvhnode::leftOrRight = leftOrRight;
}

const vector<glm::vec4> &bvhnode::getIndices() const {
    return indices;
}

void bvhnode::setIndices(const vector<glm::vec4> &indices) {
    bvhnode::indices = indices;
}

const int &bvhnode::getNumberOfPolygonsInModel() {
    return numberOfPolygonsInModel;
}

int &bvhnode::getNumberOfPolyInTheLeafWithLargestNumberOfPoly() {
    return numberOfPolyInTheLeafWithLargestNumberOfPoly;
}

void bvhnode::setNumberOfPolyInTheLeafWithLargestNumberOfPoly(int &numberOfPolyInTheLeafWithLargestNumberOfPoly) {
    bvhnode::numberOfPolyInTheLeafWithLargestNumberOfPoly = numberOfPolyInTheLeafWithLargestNumberOfPoly;
}
