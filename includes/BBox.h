//
// Created by fox-1942 on 5/11/20.
//

#ifndef RAYTRACERBOROS_BBOX_H
#define RAYTRACERBOROS_BBOX_H

#include "../Vendor/glm/vec3.hpp"
#include <vector>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

using namespace std;

struct BBox {
    static const vector<glm::vec4> &primitiveCoordinates;

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

glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2) {
    return glm::vec3(float((vec.x + vec1.x + vec2.x) / 3), float((vec.y + vec1.y + vec2.y) / 3),
                     float((vec.z + vec1.z + vec2.z) / 3));
}

glm::vec3 getCoordinatefromIndices(float index) {
    return BBox::primitiveCoordinates.at(index);
}

BBox getBBox(vector<glm::vec4> &indices) {
    vector<glm::vec3> faceCenters;
    glm::vec3 center(0, 0, 0);

    float minX = 99999, maxX = -99999;
    float minY = 99999, maxY = -99999;
    float minZ = 99999, maxZ = -99999;

    glm::vec3 firstCoordofTri;
    glm::vec3 secondCoordofTri;
    glm::vec3 thirdCoordofTri;


    for (const glm::vec4 index : indices) {

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




#endif //RAYTRACERBOROS_BBOX_H
