#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include <string>
#include <vector>

class JointTransformData {
    public:
        std::string joint_name_id;
        glm::mat4 joint_local_transform;

        JointTransformData(std::string joint_name_id, glm::mat4 joint_local_transform) {
            this->joint_name_id = joint_name_id;
            this->joint_local_transform = joint_local_transform;
        }
};
