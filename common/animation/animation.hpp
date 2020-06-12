#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include "key_frame.hpp"

class Animation {
    public:
        float duration; // Length in seconds
        std::vector<KeyFrame> frames;

        Animation(std::vector<KeyFrame> frames, float duration) {
            this->frames = frames;
            this->duration = duration;
        }
};
