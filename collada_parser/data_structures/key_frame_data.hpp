#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#ifndef JOINTTRANSFORMDATAINCLUDE
#define JOINTTRANSFORMDATAINCLUDE
#include "joint_transform_data.hpp"
#endif

#include <string>
#include <vector>

class KeyFrameData {
    public:
        float time_stamp;
        std::vector<JointTransformData> joint_transforms;

        KeyFrameData(float time_stamp) {
            this->time_stamp = time_stamp;
        }

        KeyFrameData(float time_stamp, std::vector<JointTransformData> joint_transforms) {
            this->time_stamp = time_stamp;
            this->joint_transforms = joint_transforms;
        }

        void add_joint_transform(JointTransformData joint_transform) {
            this->joint_transforms.push_back(joint_transform);
        }
};
