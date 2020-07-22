#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#ifndef JOINTTRANSFORMINCLUDE
#define JOINTTRANSFORMINCLUDE
#include "joint_transform.hpp"
#endif
#include <vector>
#include <map>
#include <string>

class KeyFrame {
    public:
        std::map<std::string, JointTransform> pose;
        float time_stamp;

        KeyFrame() {}

        KeyFrame(float time_stamp, std::map<std::string, JointTransform> joint_transforms) {
            this->time_stamp = time_stamp;
            this->pose = joint_transforms;
        }
};
