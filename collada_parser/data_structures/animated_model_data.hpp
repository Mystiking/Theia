#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#ifndef SKELETONDATAINCLUDE
#define SKELETONDATAINCLUDE
#include "skeleton_data.hpp"
#endif

#include <string>
#include <vector>

class AnimatedModelData {
    public:

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::ivec3> joint_ids;
        std::vector<glm::vec3> skinning_weights;

        SkeletonData joints;

        AnimatedModelData(
            SkeletonData joints,
            std::vector<glm::vec3> vertices,
            std::vector<glm::vec2> uvs,
            std::vector<glm::vec3> normals,
            std::vector<glm::ivec3> joint_ids,
            std::vector<glm::vec3> skinning_weights
        ) {
            this->joints = joints;

            this->vertices = vertices;
            this->uvs = uvs;
            this->normals = normals;
            this->joint_ids = joint_ids;
            this->skinning_weights = skinning_weights;
        }

};
