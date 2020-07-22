#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include <string>
#include <vector>

class JointData {
    public:
        int index;
        std::string name_id;
        glm::mat4 bind_local_transform;

        std::vector<JointData> children;

        JointData() {}

        JointData(int index, std::string name_id, glm::mat4 bind_local_transform) {
            this->index = index;
            this->name_id = name_id;
            this->bind_local_transform = bind_local_transform;
        }

        void add_child(JointData child) {
            this->children.push_back(child);
        }

};
