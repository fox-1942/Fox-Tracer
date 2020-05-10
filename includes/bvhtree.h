//
// Created by fox-1942 on 4/23/20.
//

#ifndef RAYTRACERBOROS_BVHTREE_H
#define RAYTRACERBOROS_BVHTREE_H

#define GLM_FORCE_SWIZZLE
#include "../Vendor/glm/glm.hpp"
#include <iostream>
#include <map>
#include <vector>
#include "../Vendor/glm/vec3.hpp"
#include <deque>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

using namespace std;

struct BBox {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    int longestAxis;
    vector<glm::vec3> faceCenters;

    BBox() {}

    BBox(glm::vec3 min, glm::vec3 max, glm::vec3 center, vector<glm::vec3> faceCenters) {
        this->min = min;
        this->max = max;
        this->center = center;
        this->faceCenters = faceCenters;

        float length = max.x - min.x;
        float width = max.y - min.y;
        float height = max.z - min.z;

        if (length > width && length > height) { longestAxis = 0; }
        else if (width > length && width > height) { longestAxis = 1; }
        else { longestAxis = 2; };
    }
};

int indDef = 0;
int numberOf = 1;
int numberOfLeaves = 0;

class BvhNode {
public:
    static const vector<glm::vec3> &primitiveCoordinates;

private:

    BBox bBox;
    int depthOfNode;
    vector<BvhNode *> children;
    int order;
    bool isLeaf;
    bool createdEmpty = false;
    vector<glm::vec3> indices;

public:
    BvhNode() {}

    ~BvhNode() {}

    void buildTree(vector<glm::vec3> &indicesPerFaces, int depth) {

        // This is a leaf node.
        if (indicesPerFaces.size() <= 2) {
            this->bBox = BBox();
            this->indices = indicesPerFaces;
            this->depthOfNode = depth;
            this->isLeaf = true;
            this->order = indDef;
            this->createdEmpty = false;
            indDef++;
            return;
        }

        this->depthOfNode = depth;
        this->bBox = getBBox(indicesPerFaces);
        this->isLeaf = false;
        this->order = indDef;
        indDef++;

        int axis = this->bBox.longestAxis;

        vector<glm::vec3> leftTree;
        vector<glm::vec3> rightTree;

        for (int i = 0; i < indicesPerFaces.size(); ++i) {
            switch (axis) {
                case 0:
                    if (bBox.center.x > this->bBox.faceCenters.at(i).x) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        leftTree.push_back(indicesPerFaces.at(i));

                    } else if (bBox.center.x < this->bBox.faceCenters.at(i).x) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    } else if (bBox.center.x == this->bBox.faceCenters.at(i).x) {
                        cout << "Gáz van" << endl;
                    }
                    break;
                case 1:
                    if (bBox.center.y > this->bBox.faceCenters.at(i).y) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        leftTree.push_back(indicesPerFaces.at(i));

                    } else if (bBox.center.y < this->bBox.faceCenters.at(i).y) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    } else if (bBox.center.y == this->bBox.faceCenters.at(i).y) {
                        cout << "Gáz van" << endl;
                    }
                    break;
                case 2:
                    if (bBox.center.z > this->bBox.faceCenters.at(i).z) {
                        leftTree.push_back(indicesPerFaces.at(i));
                    } else if (bBox.center.z < this->bBox.faceCenters.at(i).z) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    } else if (bBox.center.z == this->bBox.faceCenters.at(i).z) {
                        cout << "Gáz van" << endl;
                        break;
                    }
            };
        }

        cout << "Left: " << leftTree.size() << endl;
        cout << "Right: " << rightTree.size() << endl;

        BvhNode *left = new BvhNode();
        BvhNode *right = new BvhNode();

        left->buildTree(leftTree, this->depthOfNode + 1);
        right->buildTree(rightTree, this->depthOfNode + 1);

        children.push_back(left);
        children.push_back(right);

        return;
    }


private:
    glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2) {
        return glm::vec3(float((vec.x + vec1.x + vec2.x) / 3), float((vec.y + vec1.y + vec2.y) / 3),
                         float((vec.z + vec1.z + vec2.z) / 3));
    }

    glm::vec3 getCoordinatefromIndices(float index) {
        return primitiveCoordinates.at(index);
    }

    BBox getBBox(vector<glm::vec3> &indices) {
        vector<glm::vec3> faceCenters;
        glm::vec3 center(0, 0, 0);

        float minX = 99999, maxX = -99999;
        float minY = 99999, maxY = -99999;
        float minZ = 99999, maxZ = -99999;

        glm::vec3 firstCoordofTri;
        glm::vec3 secondCoordofTri;
        glm::vec3 thirdCoordofTri;


        for (const glm::vec3 index : indices) {

            firstCoordofTri = getCoordinatefromIndices(index.x);
            secondCoordofTri = getCoordinatefromIndices(index.y);
            thirdCoordofTri = getCoordinatefromIndices(index.z);

            vector<glm::vec3> currentTri;
            currentTri.push_back(firstCoordofTri);
            currentTri.push_back(secondCoordofTri);
            currentTri.push_back(thirdCoordofTri);

            for (int j = 0; j < currentTri.size(); j++) {

                minX = MIN(currentTri.at(j).x, minX);
                minY = MIN(currentTri.at(j).y, minY);
                minZ = MIN(currentTri.at(j).z, minZ);

                maxX = MAX(currentTri.at(j).x, maxX);
                maxY = MAX(currentTri.at(j).y, maxY);
                maxZ = MAX(currentTri.at(j).z, maxZ);
            }

            glm::vec3 centerOfCurrTri(calculateCenterofTriangle(firstCoordofTri, secondCoordofTri, thirdCoordofTri));

            faceCenters.push_back(centerOfCurrTri);

            center += centerOfCurrTri;
        }

        center /= indices.size(); // Calculating the average centroid of the bounding box.

        glm::vec3 minp(minX, minY, minZ);
        glm::vec3 maxp(maxX, maxY, maxZ);

        return BBox(minp, maxp, center, faceCenters);
    }


    int getNumberOfLeaves() {
        for (int i = 0; i < this->children.size(); i++) {
            if (children.at(i)->isLeaf) {
                numberOfLeaves++;
            }
            children.at(i)->getNumberOfLeaves();
        }
        return numberOfLeaves;
    }

    int findDeep(int &deepest) {
        for (int i = 0; i < this->children.size(); i++) {
            if (this->children.at(i)->depthOfNode > deepest) {
                deepest = this->children.at(i)->depthOfNode;
            }
            children.at(i)->findDeep(deepest);
        }
        return deepest;
    }

    int getDeepestLevel() {
        int deepest = -1;
        return findDeep(deepest);
    }

    void treeComplete(int deepestLev) {
        if (this->children.empty() && this->depthOfNode != deepestLev) {
            this->isLeaf = true; // Commented, because semantically 'this' would remain a leaf;

            BvhNode *emptyNode = new BvhNode();
            emptyNode->bBox = BBox();
            emptyNode->createdEmpty = true;
            emptyNode->isLeaf = true;
            emptyNode->order = -1;
            emptyNode->depthOfNode = this->depthOfNode + 1;

            this->children.push_back(emptyNode);
            this->children.push_back(emptyNode);
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

    void makeBvHTreeComplete() {
        int deep = this->getDeepestLevel();
        this->treeComplete(deep);
    }

    int countNodes() {
        for (int i = 0; i < this->children.size(); i++) {
            numberOf++;
            children.at(i)->countNodes();
        }
        return numberOf;
    }

public:

    int getNumberOfNodes() {
        numberOf = 1;
        return countNodes();
    }

    void InfoAboutNode() {
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

    struct FlatBvhNode {
        glm::vec4 min;
        glm::vec4 max;
        int order;
        int isLeaf;
        int createdEmpty;
        int  dummy; // alignment
        std::array<glm::vec4, 2> indices;

        FlatBvhNode() { }

        FlatBvhNode(glm::vec3 min, glm::vec3 max, float ind, bool isLeaf, bool createdEmpty, vector<glm::vec3> indices) {
            this->min=glm::vec4(min.x, min.y, min.z, 1.0f);
            this->max=glm::vec4(max.x, max.y, max.z, 1.0f);
            this->order=ind;
            this->isLeaf=isLeaf;
            this->createdEmpty=createdEmpty;
            this->dummy=5;

            cout<<this->isLeaf<<endl;
            cout<<this->createdEmpty<<endl;

            for (int i = 0; i < indices.size(); i++) {
                this->indices.at(i)=glm::vec4(indices.at(i).x, indices.at(i).y, indices.at(i).z, 1);
            }
        }
    };

    FlatBvhNode nodeConverter(const BvhNode node, int ind) {
        FlatBvhNode result=FlatBvhNode( node.bBox.min, node.bBox.max, ind, node.isLeaf, node.createdEmpty, node.indices);
        return result;
    }

    vector<FlatBvhNode>* putNodeIntoArray() {
        makeBvHTreeComplete();

        deque<const BvhNode *> queue;
        queue.push_back(this);

        vector<FlatBvhNode>* nodesArray=new vector<FlatBvhNode>;

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
};

#endif //RAYTRACERBOROS_BVHTREE_H