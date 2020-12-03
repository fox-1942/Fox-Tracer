//
// Created by fox-1942 on 4/12/20.
//

#ifndef RAYTRACERBOROS_VECTORCALCULATOR_H
#define RAYTRACERBOROS_VECTORCALCULATOR_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/detail/type_vec1.hpp"
#include "glm/gtc/type_ptr.hpp"

float dot(const glm::vec3& v1, const glm::vec3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

float getLength(glm::vec3 &param) { return sqrtf(dot(param, param)); }

glm::vec3 normalize(glm::vec3 &param) { return param * (1 / getLength(param)); }

glm::vec3 cross(const glm::vec3& v1, const glm::vec3& v2) {
    return glm::vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

glm::vec4 mvpCalculator(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec4 coordinates){
    return model * view * projection * coordinates;
}

#endif //RAYTRACERBOROS_VECTORCALCULATOR_H

