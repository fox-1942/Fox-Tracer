//
// Created by fox1942 on 9/15/20.
//

#ifndef RAYTRACERBOROS_MATRIXCALCULATOR_H
#define RAYTRACERBOROS_MATRIXCALCULATOR_H


#include "glm/detail/type_vec1.hpp"
#include "glm/glm.hpp"

glm::vec4 mvpCalculator(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec4 coordinates){
    return model * view * projection * coordinates;
}

#endif //RAYTRACERBOROS_MATRIXCALCULATOR_H