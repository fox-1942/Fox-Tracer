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
    vector<glm::vec3> faceCenters;

    BBox() {}

    BBox(glm::vec3 min, glm::vec3 max, glm::vec3 center, vector<glm::vec3> faceCenters) {
        this->min = min;
        this->max = max;
        this->center = center;
        this->faceCenters=faceCenters;

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
    vector<glm::vec3> primitiveCoordinates;

    BBox bBox;
    vector<BvhNode> children;

    BvhNode() {}
    ~BvhNode(){}

    BvhNode(vector<glm::vec3> &primitiveCoordinates) {
        this->primitiveCoordinates = primitiveCoordinates;

    }

    glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2) {
        return glm::vec3(float((vec.x + vec1.x + vec2.x) / 3), float((vec.y + vec1.y + vec2.y) / 3),
                         float((vec.z + vec1.z + vec2.z) / 3));
    }

    void buildTree(vector<glm::vec3> &indicesPerFaces) {

        if (indicesPerFaces.size() == 0 ) return ;

        this->bBox = getBBox(indicesPerFaces);

        // This is a leaf node.
        cout<<"hsz-ek száma: "<<indicesPerFaces.size()<<endl;
        if (indicesPerFaces.size() == 1) {

            return;
        }

        int axis = this->bBox.longestAxis;

        vector<glm::vec3> leftTree;
        vector<glm::vec3> rightTree;


        //cout <<axis<<" "<< bBox.center.x <<" " << bBox.center.y << " "<< bBox.center.z << " VS " << bBox.center[axis] << endl;


        for (int i = 0; i < indicesPerFaces.size(); ++i) {
            switch (axis) {
                case 0:
                    if (bBox.center.x >= this->bBox.faceCenters.at(i).x) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        leftTree.push_back(indicesPerFaces.at(i));

                    } else if (bBox.center.x <= this->bBox.faceCenters.at(i).x) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    };
                case 1:
                    if (bBox.center.y >= this->bBox.faceCenters.at(i).y) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        leftTree.push_back(indicesPerFaces.at(i));

                    } else if (bBox.center.y <= this->bBox.faceCenters.at(i).y) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    };
                case 2:
                    if (bBox.center.z >= this->bBox.faceCenters.at(i).z) {
                        cout<<"bbox center z: " <<bBox.center.z << endl;
                         cout<<"node bbox facenter z: "<<this->bBox.faceCenters.at(i).z << endl;
                        leftTree.push_back(indicesPerFaces.at(i));

                    } else if (bBox.center.z <= this->bBox.faceCenters.at(i).z) {
                        // cout<< faceCenters.at(i)[axis] <<endl;
                        rightTree.push_back(indicesPerFaces.at(i));
                    };
            }

            /*
            cout << bBox.center[axis] << " VS " << node->bBox.faceCenters.at(i)[axis] << endl;
            if (bBox.center[axis] >= node->bBox.faceCenters.at(i)[axis]) {
                // cout<< faceCenters.at(i)[axis] <<endl;
                leftTree.push_back(indicesPerFaces.at(i));

            } else if (bBox.center[axis] <= node->bBox.faceCenters.at(i)[axis]) {
                // cout<< faceCenters.at(i)[axis] <<endl;
                rightTree.push_back(indicesPerFaces.at(i));
            }*/
        }

        // Because in this case there would be an empty node and that is why we return.
       /* if (rightTree.size() == indicesPerFaces.size() || leftTree.size() == indicesPerFaces.size()) {
            return;
        }*/

        cout<<rightTree.size()<<endl;
        cout<<leftTree.size()<<endl;

        BvhNode left=BvhNode(primitiveCoordinates);
        BvhNode right=BvhNode(primitiveCoordinates);
        left.buildTree(leftTree );
        right.buildTree (rightTree);

        children.push_back(left);
        children.push_back(right);

        return;
    }

    glm::vec3 getCoordinatefromIndices(float index) {
          cout << "getCoordinates-ba beléptem. " <<"Index: "<<index<<endl;
            cout<<" Primitve coordinates: "<<primitiveCoordinates.size()<< endl;
          cout <<"Koordináták: " << primitiveCoordinates.at(index).x << " " << primitiveCoordinates.at(index).y << " "
               << primitiveCoordinates.at(index).z << endl;

        return primitiveCoordinates.at(index);
    }

    BBox getBBox(vector<glm::vec3>& indices) {
        vector<glm::vec3> faceCenters;
        glm::vec3 center(0, 0, 0);

        float minX = 99999, maxX = -99999;
        float minY = 99999, maxY = -99999;
        float minZ = 99999, maxZ = -99999;

        glm::vec3 firstCoordofTri;
        glm::vec3 secondCoordofTri;
        glm::vec3 thirdCoordofTri;


        for (const glm::vec3 index : indices) {

            cout<<"Indices size: "<<indices.size()<<endl;
            cout<<"Index.x "<<index.x<<endl;
            firstCoordofTri = getCoordinatefromIndices(index.x);
            cout<<"utána"<<endl;
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

        /* cout << center.x << " " << center.y << " " << center.z << endl;

         cout << "Min of bbox: " << minp.x << " " << minp.y << " " << minp.z << endl;
         cout << "Max of BBox: " << maxp.x << " " << maxp.y << " " << maxp.z << endl;*/

        return BBox(minp, maxp, center, faceCenters);
    }
};

#endif //RAYTRACERBOROS_BVHTREE_H