//
// Created by fox-1942 on 4/23/20.
//

#ifndef RAYTRACERBOROS_KDTREE_H
#define RAYTRACERBOROS_KDTREE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "../Vendor/glm/vec4.hpp"
#include "../Vendor/glm/vec3.hpp"

using namespace std;

struct BBox {
    glm::vec4 min;
    glm::vec4 max;
    glm::vec4 center;
    int longestAxis;

    float length = max.x - min.x;
    float width = max.y - min.y;
    float height = max.z - min.z;

    BBox(glm::vec4 min, glm::vec4 max, glm::vec4 center) {
        this->min = min;
        this->max = max;
        this->center = center;
        if (length > width && length > height) longestAxis = 0;
        else if (width > length && width > height) longestAxis = 1;
        else longestAxis = 2;
    }
};

class KdNode {

public:
    vector<glm::vec4> primitiveCoordinates;
    vector<glm::vec4> faceCenters;

    KdNode *right;
    KdNode *left;

    KdNode(vector<glm::vec4> &primitiveCoordinates) {
        this->primitiveCoordinates = primitiveCoordinates;
        this->left = NULL;
        this->right = NULL;
    }









    glm::vec4 calculateCenterofTriangle(glm::vec4 vec, glm::vec4 vec1, glm::vec4 vec2) {
        return glm::vec4((vec.x + vec1.x + vec2.x) / 3, (vec.y + vec1.y + vec2.y) / 3, (vec.z + vec1.z + vec2.z) / 3,
                         1.0);
    }







    KdNode *buildTree(vector<glm::vec4> indicesPerFaces, int depth) {
        if (indicesPerFaces.size() == 0 || primitiveCoordinates.size() == 0) return NULL;

        BBox bBox = getBBox(indicesPerFaces);
        int axis = bBox.longestAxis;


        vector<glm::vec4> leftTree;
        vector<glm::vec4> rightTree;

        for (int i = 0; i < indicesPerFaces.size(); ++i) {
            if (bBox.center[axis] >= faceCenters.at(i)[axis]) {
                leftTree.push_back(indicesPerFaces.at(i));
            } else if (bBox.center[axis] <= faceCenters.at(i)[axis]) {
                rightTree.push_back(indicesPerFaces.at(i));
            }
        }

        if(rightTree.size() == indicesPerFaces.size() || leftTree.size() == indicesPerFaces.size()){
            return this;
        }

        this->left = buildTree(leftTree, depth + 1);
        this->right = buildTree(rightTree, depth + 1);

        return this;
    }







    glm::vec4 getCoordinatefromIndices(float index) {
        glm::vec4 back;
        for (int i = 0; i < primitiveCoordinates.size(); i++) {
            if (i == index) {
                back = primitiveCoordinates[i];
                break;
            }
        }
        return back;
    }








    BBox getBBox(vector<glm::vec4> indicesPerFaces) {
        glm::vec4 center(0, 0, 0, 1);

        float minX = 1e9, maxX = 1e-9;
        float minY = 1e9, maxY = 1e-9;
        float minZ = 1e9, maxZ = 1e-9;

        glm::vec4 firstCoordofTri;
        glm::vec4 secondCoordofTri;
        glm::vec4 thirdCoordofTri;

        for (int i = 0; i < indicesPerFaces.size(); ++i) {
            firstCoordofTri = getCoordinatefromIndices(indicesPerFaces.at(i).x);
            secondCoordofTri = getCoordinatefromIndices(indicesPerFaces.at(i).y);
            thirdCoordofTri = getCoordinatefromIndices(indicesPerFaces.at(i).z);

            vector<glm::vec4> currentTri;
            currentTri.push_back(firstCoordofTri);
            currentTri.push_back(secondCoordofTri);
            currentTri.push_back(thirdCoordofTri);

            for (int j = 0; j < currentTri.size(); j++) {
                if (currentTri.at(j).x < minX) { minX = currentTri.at(j).x; }
                if (currentTri.at(j).y < minY) { minY = currentTri.at(j).y; }
                if (currentTri.at(j).z < minZ) { minZ = currentTri.at(j).z; }

                if (currentTri.at(j).x > maxX) { maxX = currentTri.at(j).x; }
                if (currentTri.at(j).y > maxY) { maxY = currentTri.at(j).y; }
                if (currentTri.at(j).z > maxZ) { maxZ = currentTri.at(j).z; }
            }

            glm::vec4 centerOfCurrTri(calculateCenterofTriangle(firstCoordofTri, secondCoordofTri, thirdCoordofTri));
            faceCenters.push_back(centerOfCurrTri);
            center += centerOfCurrTri;
        }

        center /= 3; // Calculating the average centroid of the bounding box.

        glm::vec4 minp(minX, minY, minZ, 1.0);
        glm::vec4 maxp(maxX, maxY, maxZ, 1.0);

        return BBox(minp, maxp, center);
    }
};

#endif //RAYTRACERBOROS_KDTREE_H
