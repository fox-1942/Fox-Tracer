//
// Created by fox-1942 on 4/12/20.
//

#ifndef RAYTRACERBOROS_VECTORCALCULATOR_H
#define RAYTRACERBOROS_VECTORCALCULATOR_H

#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../Vendor/glm/gtc/type_ptr.hpp"

float dot( glm::vec3& param1,  glm::vec3& param2) { return (param1.x * param2.x + param1.y * param2.y + param1.z * param2.z); }

float getLength(glm::vec3& param) { return sqrtf(dot(param, param)); }

glm::vec3 normalize( glm::vec3& param) { return param * (1 / getLength(param)); }

glm::vec3 cross( glm::vec3& param1,  glm::vec3& param2) {
    return normalize(glm::vec3(param1.y * param2.z - param1.z * param2.y, param1.z * param2.x - param1.x * param2.z, param1.x * param2.y - param1.y * param2.x));
}

#endif //RAYTRACERBOROS_VECTORCALCULATOR_H

