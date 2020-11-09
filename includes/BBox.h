//
// Created by fox-1942 on 5/11/20.
//

#ifndef RAYTRACERBOROS_BBOX_CPP
#define RAYTRACERBOROS_BBOX_CPP

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <vector>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

using namespace std;

class BBox {

    static const vector<glm::vec4> &primitiveCoordinates;
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    int longestAxis;
    vector<glm::vec3> faceCenters;


public:
    static const vector<glm::vec4> &getPrimitiveCoordinates();

    const glm::vec3 &getMin() const;

    void setMin(const glm::vec3 &min);

    const glm::vec3 &getMax() const;

    void setMax(const glm::vec3 &max);

    const glm::vec3 &getCenter() const;

    void setCenter(const glm::vec3 &center);

    int getLongestAxis() const;

    void setLongestAxis(int longestAxis);

    const vector<glm::vec3> &getFaceCenters() const;

    void setFaceCenters(const vector<glm::vec3> &faceCenters);


public:
    BBox();

    BBox(glm::vec3 min, glm::vec3 max, glm::vec3 center, vector<glm::vec3> faceCenters);

    glm::vec3 calculateCenterofTriangle(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2);

    glm::vec3 getCoordinatesfromIndex(float index);

    BBox getBBox(vector<glm::vec4> &indices);
};

#endif //RAYTRACERBOROS_BBOX_CPP
