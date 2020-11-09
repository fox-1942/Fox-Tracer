//
// Created by fox-1942 on 4/12/20.
//

#ifndef RAYTRACERBOROS_VECTORCALCULATOR_H
#define RAYTRACERBOROS_VECTORCALCULATOR_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/detail/type_vec1.hpp"
#include "glm/gtc/type_ptr.hpp"

float dot(glm::vec3 &param1, glm::vec3 &param2) {
    return (param1.x * param2.x + param1.y * param2.y + param1.z * param2.z);
}

float getLength(glm::vec3 &param) { return sqrtf(dot(param, param)); }

glm::vec3 normalize(glm::vec3 &param) { return param * (1 / getLength(param)); }

glm::vec3 cross(glm::vec3 &param1, glm::vec3 &param2) {
    return normalize(glm::vec3(param1.y * param2.z - param1.z * param2.y, param1.z * param2.x - param1.x * param2.z,
                               param1.x * param2.y - param1.y * param2.x));
}

glm::vec4 mvpCalculator(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec4 coordinates){
    return model * view * projection * coordinates;
}

#endif //RAYTRACERBOROS_VECTORCALCULATOR_H

