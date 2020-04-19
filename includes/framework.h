//
// Created by fox-1942 on 4/12/20.
//

#ifndef RAYTRACERBOROS_FRAMEWORK_H
#define RAYTRACERBOROS_FRAMEWORK_H

#endif //RAYTRACERBOROS_FRAMEWORK_H

#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../Vendor/glm/gtc/type_ptr.hpp"

inline float dot(const glm::vec3& v1, const glm::vec3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

inline float length(const glm::vec3& v) { return sqrtf(dot(v, v)); }

inline glm::vec3 normalize(const glm::vec3& v) { return v * (1 / length(v)); }

inline glm::vec3 cross(const glm::vec3& v1, const glm::vec3& v2) {
    return glm::vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline glm::vec3 mulProject(glm::vec3 resultVec3, glm::mat4x4 mat) {
    float x = resultVec3.x;
    float y = resultVec3.y;
    float z = resultVec3.z;
    float invW = 1.0F / glm::fma(mat[0][3], x, glm::fma(mat[1][3], y, glm::fma(mat[2][3], z, mat[3][3])));
    resultVec3.x = glm::fma(mat[0][0], x, glm::fma(mat[1][0], y, glm::fma(mat[2][0], z, mat[3][0]))) * invW;
    resultVec3.y = glm::fma(mat[0][1], x, glm::fma(mat[1][1], y, glm::fma(mat[2][1], z, mat[3][1]))) * invW;
    resultVec3.z = glm::fma(mat[0][2], x, glm::fma(mat[1][2], y, glm::fma(mat[2][2], z, mat[3][2]))) * invW;
    return resultVec3;
}

inline glm::vec3 sub(glm::vec3 a, glm::vec3 v) {
    a.x -= v.x;
    a.y -= v.y;
    a.z -= v.z;
    return a;
}
