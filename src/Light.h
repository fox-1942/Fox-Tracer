//
// Created by fox-1942 on 3/17/20.
//

#include "../Vendor/glm/glm.hpp"

#ifndef OPENGL_LIGHT_H
#define OPENGL_LIGHT_H

#endif //OPENGL_LIGHT_H

class Light {
private:
    glm::vec3 direction;
    glm::vec3 Le, La;

public:
    Light(glm::vec3 _direction, glm::vec3 _Le, glm::vec3 _La) {
        direction = normalize(_direction);
        Le = _Le;
        La = _La;
    }

public:
    const glm::vec3 &getDirection() const {
        return direction;
    }

    void setDirection(const glm::vec3 &direction) {
        Light::direction = direction;
    }

    const glm::vec3 &getLe() const {
        return Le;
    }

    void setLe(const glm::vec3 &le) {
        Le = le;
    }

    const glm::vec3 &getLa() const {
        return La;
    }

    void setLa(const glm::vec3 &la) {
        La = la;
    }
};