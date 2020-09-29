#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include <string>

class Bone {
    public:
        int index;                       // Bone index
        glm::quat q_wcs;                 // Joint frame orientation in World Coordinate Space
        glm::vec3 t_wcs;                 // Joint origin in World Coordinate Space
        glm::vec3 t = glm::vec3(0.0f);   // Joint vector in parent frame
        int parent = -1;                 // Index to parent bone (-1 if bone is root)
        std::vector<int> children;       // Indices to child bones

        std::string euler_code = "ZYZ";  // Euler angle convention
        float alpha = 0.0f;              // Euler angles of bone
        float beta = 0.0f;               // Euler angles of bone
        float gamma = 0.0f;              // Euler angles of bone

        bool is_root() {
            return (this->parent == -1);
        }

        bool is_end_effector() {
            return (this->children.size() == 0);
        }

        bool has_parent() {
            return !(this->is_root());
        }

        glm::vec3 get_axis(int idx) {
            if (this->euler_code[idx] == 'X') {
                return glm::vec3(1.0f, 0.0f, 0.0f);
            }
            if (this->euler_code[idx] == 'Y') {
                return glm::vec3(0.0f, 1.0f, 0.0f);
            }
            if (this->euler_code[idx] == 'Z') {
                return glm::vec3(0.0f, 0.0f, 1.0f);
            }
        }

        glm::quat get_rotation(int idx, float radians) {
            float c = glm::cos(radians/2.0f);
            float s = glm::sin(radians/2.0f);
            if (this->euler_code[idx] == 'X') {
                return glm::quat(c, s, 0.0f, 0.0f);
            }
            if (this->euler_code[idx] == 'Y') {
                return glm::quat(c, 0.0f, s, 0.0f);
            }
            if (this->euler_code[idx] == 'Z') {
                return glm::quat(c, 0.0f, 0.0f, s);
            }
        }

        glm::vec3 get_axis_alpha() {
            return this->get_axis(0);
        }

        glm::vec3 get_axis_beta() {
            return this->get_axis(1);
        }

        glm::vec3 get_axis_gamma() {
            return this->get_axis(2);
        }

        glm::quat get_rotation_alpha() {
            return this->get_rotation(0, this->alpha);
        }

        glm::quat get_rotation_beta() {
            return this->get_rotation(1, this->beta);
        }

        glm::quat get_rotation_gamma() {
            return this->get_rotation(2, this->gamma);
        }
};
