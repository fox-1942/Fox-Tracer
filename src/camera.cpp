//
// Created by fox1942 on 11/22/20.
//

#include "camera.h"

Camera::Camera(float fieldOfview, glm::vec3 posCamera, glm::vec3 upVector, glm::vec3 viewPoint) : fieldOfview(
        fieldOfview), posCamera(posCamera), upVector(upVector), viewPoint(viewPoint) {
}

float Camera::getFieldOfview() const {
    return fieldOfview;
}

glm::vec3 &Camera::getPosCamera()  {
    return posCamera;
}

glm::vec3 &Camera::getUpVector()  {
    return upVector;
}

glm::vec3 &Camera::getViewPoint()  {
    return viewPoint;
}

void Camera::setPosCamera(const glm::vec3 &posCamera) {
    Camera::posCamera = posCamera;
}
