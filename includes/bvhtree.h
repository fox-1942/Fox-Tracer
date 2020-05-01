//
// Created by fox-1942 on 4/23/20.
//

#ifndef RAYTRACERBOROS_BVHTREE_H
#define RAYTRACERBOROS_BVHTREE_H

#include <iostream>
#include <map>
#include <vector>
#include "../Vendor/glm/vec3.hpp"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))


using namespace std;

struct BBox {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    int longestAxis;

    BBox() {}

    BBox(glm::vec3 min, glm::vec3 max, glm::vec3 center) {
        this->min = min;
        this->max = max;
        this->center = center;

        float length = max.x - min.x;
        float width = max.y - min.y;
        float height = max.z - min.z;

        /*cout<<"min: " << this->min.x<< " " <<this->min.y<<" " <<this->min.z <<endl;
        cout<<"max: " << this->max.x<< " " <<this->max.y<<" " <<this->max.z <<endl;
        cout<<"center: " << this->center.x<< " " <<this->center.y<<" " <<this->center.z <<endl;
        cout<<"length: " << length<<endl;*/

        if (length > width && length > height) { longestAxis = 0; }
        else if (width > length && width > height) { longestAxis = 1; }
        else { longestAxis = 2; };
    }
};

class BvhNode {

public:
    BvhNode() {}

    vector<glm::vec3> primitiveCoordinates;
    vector<glm::vec3> faceCenters;

    BBox bBox;
    BvhNode *right;
    BvhNode *left;
    int numberofBBoxes = 0;

    BvhNode(vector<glm::vec3> &primitiveCoordinates) {
        this->primitiveCoordinates = primitiveCoordinates;
        this->left = NULL;
        this->right = NULL;
    }

    glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2) {
        return glm::vec3(float((vec.x + vec1.x + vec2.x) / 3), float((vec.y + vec1.y + vec2.y) / 3),
                         float((vec.z + vec1.z + vec2.z) / 3));
    }


    BvhNode *buildTree(vector<glm::vec3> &indicesPerFaces, int depth) {
        BvhNode *node = new BvhNode();

        if (indicesPerFaces.size() == 0 || primitiveCoordinates.size() == 0) return NULL;

        node->bBox = getBBox(indicesPerFaces);
        node->numberofBBoxes++;
        this->numberofBBoxes++;


        if (indicesPerFaces.size() == 1) {
            return node;
        }

        int axis = node->bBox.longestAxis;

        vector<glm::vec3> leftTree;
        vector<glm::vec3> rightTree;

        glm::vec3 firstCoordofTri;
        glm::vec3 secondCoordofTri;
        glm::vec3 thirdCoordofTri;
        vector<glm::vec3> currentTri;

        cout << bBox.center.x << " " << bBox.center.x << " " << bBox.center.x << endl;
        cout << "Axis: " << axis << endl;

        for (int i = 0; i < indicesPerFaces.size(); ++i) {
            cout << bBox.center[axis] << " VS " << faceCenters.at(i)[axis] << endl;
            if (bBox.center[axis] >= faceCenters.at(i)[axis]) {
                // cout<< faceCenters.at(i)[axis] <<endl;
                leftTree.push_back(indicesPerFaces.at(i));

            } else if (bBox.center[axis] <= faceCenters.at(i)[axis]) {
                // cout<< faceCenters.at(i)[axis] <<endl;
                rightTree.push_back(indicesPerFaces.at(i));
            }
        }


        // Because in this case there would be an empty node and that is why we return.
        if (rightTree.size() == indicesPerFaces.size() || leftTree.size() == indicesPerFaces.size()) {
            return this;
        }

        depth++;
        cout << "depth: " << depth << endl;
        this->left = buildTree(leftTree, depth);
        this->right = buildTree(rightTree, depth);

        return this;
    }

    glm::vec3 getCoordinatefromIndices(float index) {
        /*  cout << "getCoordinates: " << endl;
          cout << primitiveCoordinates.at(index).x << " " << primitiveCoordinates.at(index).y << " "
               << primitiveCoordinates.at(index).z << endl;*/

        return primitiveCoordinates.at(index);
    }

    BBox getBBox(vector<glm::vec3> &indices) {

        glm::vec3 center(0, 0, 0);

        float minX = 999999999, maxX = -999999;
        float minY = 999999999, maxY = -999999;
        float minZ = 999999999, maxZ = -999999;

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
