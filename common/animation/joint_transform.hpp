#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

class JointTransform {
    public:
        glm::vec3 position;
        glm::quat rotation;

        JointTransform() {}

        JointTransform(glm::vec3 position, glm::quat rotation) {
            this->position = position;
            this->rotation = rotation;
        }

        glm::mat4 get_local_transform() {
            glm::mat4 local_transform = glm::translate(glm::mat4(1.0f), this->position);
            local_transform = glm::mat4_cast(this->rotation) * local_transform;
            return local_transform;
        }

        static JointTransform interpolate(JointTransform frame_a, JointTransform frame_b, float progression) {
            glm::vec3 position = interpolate(frame_a.position, frame_b.position, progression);
            glm::quat rotation = glm::mix(frame_a.rotation, frame_b.rotation, progression);
            return JointTransform(position, rotation);
        }

        static glm::vec3 interpolate(glm::vec3 v, glm::vec3 u, float a) {
            return glm::vec3(v.x + (u.x - v.x) * a, v.y + (u.y - v.y) * a, v.z + (u.z - v.z) * a);
        }
};
