#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include "animation.hpp"

class Animator{
    public:
        Animation *current_animation;
        float animation_time;

        Animator() {}

        void update() {}

        void do_animation(Animation *animation) {
            this->current_animation = animation;
        }
};
