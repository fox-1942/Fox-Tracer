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

class kdNode {

public:
    glm::vec4 min, max;
    vector<glm::vec4> triangles;
    kdNode *right;
    kdNode *left;
    int axis;
    vector<glm::vec4> indicesPerFaces;
    vector<glm::vec4> primitiveCoordinates;

    kdNode(vector<glm::vec4> &primitiveCoordinates, vector<glm::vec4> &indicesPerFaces) {
        this->primitiveCoordinates = primitiveCoordinates;
        this->indicesPerFaces = indicesPerFaces;
    }

private:

    glm::vec3 calculateCenterofTriangle(glm::vec4 vec) {

    }

    kdNode *buildTree(int depth) {
        if (tris.size() == 0) return NULL;

        kdNode *node = new kdNode();

        node->triangles = tris;
        node->left = NULL;
        node->right = NULL;
    }

    glm::vec3 getCoordinatefromIndices(float index) {
        glm::vec3 back;
        for (int i = 0; i < primitiveCoordinates.length(); i++) {
            if (i == index) {
                back = primitiveCoordinates[i];
                break;
            }
        }
        return back;
    }

    bBox getbBox(vector<glm::vec4> tris) {
        glm::vec3 center(0, 0, 0);

        float minX = 1e9, maxX = 1e-9;
        float minY = 1e9, maxY = 1e-9;
        float minZ = 1e9, maxZ = 1e-9;

        for (int i = 0; i < tris.size(); ++i) {
            if (tris.at(i).x < minX) { minX = tris.at(i).x; }
            if (tris.at(i).y < minY) { minY = tris.at(i).y; }
            if (tris.at(i).z < minZ) { minZ = tris.at(i).z; }

            if (tris.at(i).x < maxX) { maxX = tris.at(i).x; }
            if (tris.at(i).y < maxY) { maxY = tris.at(i).y; }
            if (tris.at(i).z < maxZ) { maxZ = tris.at(i).z; }

            center += calculateCenterofTriangle(tris.at(i));
        }
        center = glm::vec3(center.x / tris.size(),center.y / tris.size(),
                           center.z / tris.size());

        glm::vec3 minp(minX, minY, minZ);
        glm::vec3 maxp(maxX, maxY, maxZ);
        bBox bBox(minp, maxp, center);
        return bBox;
    }
};

#endif //RAYTRACERBOROS_KDTREE_H
