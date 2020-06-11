#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include "joint_transform.hpp"
#include <vector>

class KeyFrame {
    public:
        std::vector<JointTransform> joint_transforms;
        float time_stamp;

        KeyFrame() {}
};
