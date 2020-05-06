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

template<typename T>
void pop_front(std::vector<T>& vec)
{
    assert(!vec.empty());
    vec.erase(vec.begin());
}