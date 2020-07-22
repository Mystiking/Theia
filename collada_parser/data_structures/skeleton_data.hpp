#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#ifndef JOINTDATAINCLUDE
#define JOINTDATAINCLUDE
#include "joint_data.hpp"
#endif

#include <string>
#include <vector>

class SkeletonData {
    public:
        int joint_count;
        JointData root_joint;

        SkeletonData() {}

        SkeletonData(int joint_count, JointData root_joint) {
            this->joint_count = joint_count;
            this->root_joint = root_joint;
        }
};
