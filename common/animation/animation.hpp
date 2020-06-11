#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include "key_frame.hpp"

class Animation {
    std::vector<KeyFrame> frames;

    Animation(std::vector<KeyFrame> frames) {
        this->frames = frames;
    }
};
