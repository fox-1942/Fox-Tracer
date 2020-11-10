//
// Created by fox-1942 on 4/23/20.
//

#ifndef RAYTRACERBOROS_BVHNODE_H
#define RAYTRACERBOROS_BVHNODE_H

#include <iostream>
#include <map>
#include <vector>
#include <deque>

#include "bbox.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"

using namespace std;

class bvhnode {

private:

    static const int &numberOfPolygonsInModel;
    static int &numberOfPolyInTheLeafWithLargestNumberOfPoly;

    BBox bBox;
    int depthOfNode;
    vector<bvhnode *> children;
    int order;
    bool isLeaf;
    bool createdEmpty = false;
    int leftOrRight;
    vector<glm::vec4> indices;

public:

    bvhnode();

    ~bvhnode();

    void buildTree(vector<glm::vec4> &indices, int depth);

    void makeBvHTreeComplete();

    void InfoAboutNode();

private:

    int countNodes() const;

    int getNumberOfNodes();

    int getNumberOfLeaves();

    int findDeep(int &deepest);

    int getDeepestLevel();

    void treeComplete(int deepestLev);

public:

    const BBox &getBBox() const;

    void setBBox(const BBox &bBox);

    int getDepthOfNode() const;

    void setDepthOfNode(int depthOfNode);

    const vector<bvhnode *> &getChildren() const;

    void setChildren(const vector<bvhnode *> &children);

    int getOrder() const;

    void setOrder(int order);

    bool getIsLeaf() const;

    void setIsLeaf(bool isLeaf);

    bool isCreatedEmpty() const;

    void setCreatedEmpty(bool createdEmpty);

    int getLeftOrRight() const;

    void setLeftOrRight(int leftOrRight);

    const vector<glm::vec4> &getIndices() const;

    void setIndices(const vector<glm::vec4> &indices);

    static const int &getNumberOfPolygonsInModel();

    static int &getNumberOfPolyInTheLeafWithLargestNumberOfPoly();

    static void setNumberOfPolyInTheLeafWithLargestNumberOfPoly(int &numberOfPolyInTheLeafWithLargestNumberOfPoly);
};

#endif //RAYTRACERBOROS_BVHNODE_H