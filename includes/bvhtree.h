//
// Created by fox-1942 on 4/23/20.
//

#ifndef RAYTRACERBOROS_BVHTREE_H
#define RAYTRACERBOROS_BVHTREE_H

#include <iostream>
#include <map>
#include <vector>
#include "../Vendor/glm/vec3.hpp"

using namespace std;

struct BBox {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    int longestAxis;

    float length = max.x - min.x;
    float width = max.y - min.y;
    float height = max.z - min.z;

    BBox(glm::vec3 min, glm::vec3 max, glm::vec3 center) {
        this->min = min;
        this->max = max;
        this->center = center;
        if (length > width && length > height) longestAxis = 0;
        else if (width > length && width > height) longestAxis = 1;
        else longestAxis = 2;
    }

    BBox() {}
};

class BvhNode {

public:
    BvhNode() {

    }

    vector<glm::vec3> primitiveCoordinates;
    vector<glm::vec3> faceCenters;

    BBox bBox;
    BvhNode *right;
    BvhNode *left;
    int numberofBBoxes=0;

    BvhNode(vector<glm::vec3> &primitiveCoordinates) {
        this->primitiveCoordinates = primitiveCoordinates;
        this->left = NULL;
        this->right = NULL;
    }

    glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2) {
        return glm::vec3((vec.x + vec1.x + vec2.x) / 3, (vec.y + vec1.y + vec2.y) / 3, (vec.z + vec1.z + vec2.z) / 3);
    }


    BvhNode *buildTree(vector<glm::vec3> &indicesPerFaces, int depth) {
        BvhNode* node=new BvhNode();

        node->left=NULL;
        node->right=NULL;
        node->bBox=BBox();

        if (indicesPerFaces.size() == 0 || primitiveCoordinates.size() == 0) return node;

        if(indicesPerFaces.size()==1){
            node->bBox=BBox();
            node->left=new BvhNode();
            node->right=new BvhNode();
            return node;
        }

        node->bBox=getBBox(indicesPerFaces);


        bBox = getBBox(indicesPerFaces);

        int axis = bBox.longestAxis;


        vector<glm::vec3> leftTree;
        vector<glm::vec3> rightTree;

        cout << "face-ek száma: " << indicesPerFaces.size() << endl;
        cout << "faceCenterek-ek száma: " << faceCenters.size() << endl;

        cout << "bbox center: " << bBox.center.x<< " "<<bBox.center.y <<" "<< bBox.center.z << endl;
        cout << "bbox center: " << bBox.center[axis] << endl;

        for (int i = 0; i < indicesPerFaces.size(); ++i) {
            if (bBox.center[axis] >= faceCenters.at(i)[axis]) {
                cout<< faceCenters.at(i)[axis] <<endl;
                leftTree.push_back(indicesPerFaces.at(i));

            } else if (bBox.center[axis] <= faceCenters.at(i)[axis]) {
                cout<< faceCenters.at(i)[axis] <<endl;
                rightTree.push_back(indicesPerFaces.at(i));
            }
        }

        cout << endl;

        if (rightTree.size() == indicesPerFaces.size() || leftTree.size() == indicesPerFaces.size()) {
            //return this;
        }

        this->left = buildTree(leftTree, depth + 1);
        this->right = buildTree(rightTree, depth + 1);

        cout<<"Boxok száma: "<<numberofBBoxes;
        return this;
    }

    glm::vec3 getCoordinatefromIndices(float index) {
        return primitiveCoordinates.at((int) index);
    }


    BBox getBBox(vector<glm::vec3>& indices) {
        this->numberofBBoxes++;
        glm::vec3 center(0, 0, 0);

        float minX = 1e9, maxX = -1e9;
        float minY = 1e9, maxY = -1e9;
        float minZ = 1e9, maxZ = -1e9;

        glm::vec3 firstCoordofTri;
        glm::vec3 secondCoordofTri;
        glm::vec3 thirdCoordofTri;
        vector<glm::vec3> currentTri;

        for (int i = 0; i < indices.size(); ++i) {
            firstCoordofTri = getCoordinatefromIndices(indices.at(i).x);
            secondCoordofTri = getCoordinatefromIndices(indices.at(i).y);
            thirdCoordofTri = getCoordinatefromIndices(indices.at(i).z);

            currentTri.push_back(firstCoordofTri);
            currentTri.push_back(secondCoordofTri);
            currentTri.push_back(thirdCoordofTri);

            for (int j = 0; j < currentTri.size(); j++) {
                if (currentTri.at(j).x < minX) {
                    minX = currentTri.at(j).x;
                }
                if (currentTri.at(j).y < minY) {
                    minY = currentTri.at(j).y;
                }
                if (currentTri.at(j).z < minZ) {
                    minZ = currentTri.at(j).z;
                }

                if (currentTri.at(j).x > maxX) {
                    maxX = currentTri.at(j).x;
                }
                if (currentTri.at(j).y > maxY) {
                    maxY = currentTri.at(j).y;
                }
                if (currentTri.at(j).z > maxZ) {
                    maxZ = currentTri.at(j).z;
                }
            }

            glm::vec3 centerOfCurrTri(calculateCenterofTriangle(firstCoordofTri, secondCoordofTri, thirdCoordofTri));

            faceCenters.push_back(centerOfCurrTri);
            center += centerOfCurrTri;
        }

        center /= 3; // Calculating the average centroid of the bounding box.

        glm::vec3 minp(minX, minY, minZ);
        glm::vec3 maxp(maxX, maxY, maxZ);

       /* cout << center.x << " " << center.y << " " << center.z << endl;

        cout << "Min of bbox: " << minp.x << " " << minp.y << " " << minp.z << endl;
        cout << "Max of BBox: " << maxp.x << " " << maxp.y << " " << maxp.z << endl;*/

        return BBox(minp, maxp, center);
    }



};

#endif //RAYTRACERBOROS_BVHTREE_H
