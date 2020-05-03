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
    int longestAxis = 0;
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

class BvhNode {

public:
    vector<glm::vec3> primitiveCoordinates;

    BBox bBox;
    vector<BvhNode> children;

    BvhNode() {}

    ~BvhNode() {}

    BvhNode(vector<glm::vec3> &primitiveCoordinates) {
        this->primitiveCoordinates = primitiveCoordinates;

    }

    glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2) {
        return glm::vec3(float((vec.x + vec1.x + vec2.x) / 3), float((vec.y + vec1.y + vec2.y) / 3),
                         float((vec.z + vec1.z + vec2.z) / 3));
    }

    void buildTree(vector<glm::vec3> &indicesPerFaces) {

        this->bBox = getBBox(indicesPerFaces);

        // This is a leaf node.
        if (indicesPerFaces.size() == 2 ) {
            return;
        }

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
                        cout<<"Gáz van"<<endl;
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
                        cout<<"Gáz van"<<endl;
                    }
                    break;
                case 2:
                    if (bBox.center.z > this->bBox.faceCenters.at(i).z) {
                        leftTree.push_back(indicesPerFaces.at(i));
                    } else if (bBox.center.z < this->bBox.faceCenters.at(i).z) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    } else if (bBox.center.z == this->bBox.faceCenters.at(i).z) {
                        cout<<"Gáz van"<<endl;
                        break;
                    }
            };
        }

        cout << "Left: " << leftTree.size() << endl;
        cout << "Right: " << rightTree.size() << endl;


        BvhNode left = BvhNode(primitiveCoordinates);
        BvhNode right = BvhNode(primitiveCoordinates);
        left.buildTree(leftTree);
        right.buildTree(rightTree);

        children.push_back(left);
        children.push_back(right);

        return;
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
};

#endif //RAYTRACERBOROS_BVHTREE_H