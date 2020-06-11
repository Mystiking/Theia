#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

class JointTransform {
    public:
        glm::vec3 position;
        glm::quat rotation;

        JointTransform() {}

        JointTransform(glm::vec3 position, glm::quat rotation) {
            this->position = position;
            this->rotation = rotation;
        }
};
