//
// Created by fox1942 on 11/22/20.
//

#ifndef RAYTRACERBOROS_CAMERA_H
#define RAYTRACERBOROS_CAMERA_H

#include <glm/vec3.hpp>

class Camera {
public:
    float fieldOfview;
    glm::vec3 posCamera;
    glm::vec3 upVector;
    glm::vec3 viewPoint;

public:
    float getFieldOfview() const;

    void setPosCamera(const glm::vec3 &posCamera);

    glm::vec3 &getPosCamera();

    glm::vec3 &getUpVector();

    glm::vec3 &getViewPoint();

    Camera()=default;

    Camera(float fieldOfview, glm::vec3 posCamera, glm::vec3 upVector, glm::vec3 viewPoint);
};


#endif //RAYTRACERBOROS_CAMERA_H
