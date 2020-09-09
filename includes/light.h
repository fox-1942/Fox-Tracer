//
// Created by fox-1942 on 5/21/20.
//

#ifndef RAYTRACERBOROS_LIGHT_H
#define RAYTRACERBOROS_LIGHT_H

#include "glm/vec3.hpp"
#include "vectorCalculator.h"

struct Light {
    glm::vec3 Le;
    glm::vec3 La;
    glm::vec3 direction;
    glm::vec3 position;

    Light(glm::vec3 direction_, glm::vec3 Le_, glm::vec3 La_) {
        direction = normalize(direction_);
        Le = Le_;
        La = La_;
    }
};

#endif //RAYTRACERBOROS_LIGHT_H
