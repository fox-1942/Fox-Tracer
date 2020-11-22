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


class BvhNode {

private:

    static const int &numberOfPolygonsInModel;
    static int &numberOfPolyInTheLeafWithLargestNumberOfPoly;

    BBox bBox;
    int depthOfNode;
    int order;
    bool isLeaf;
    bool createdEmpty;
    int leftOrRight;

    vector<BvhNode *> children;
    vector<glm::vec4> indices;

public:

    BvhNode();

    ~BvhNode();

    void buildTree(vector<glm::vec4> &indices, int depth);

    void makeBvHTreeComplete();

    void InfoAboutNode();

private:

    int countNodes();

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

    const vector<BvhNode *> &getChildren() const;

    void setChildren(const vector<BvhNode *> &children);

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