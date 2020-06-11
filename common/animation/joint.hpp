#include <vector>
#include <string>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#endif

class Joint {
    public:
        Joint() {}

        Joint(int id, std::string name, glm::mat4 transform) {
            this->id = id;
            this->name = name;
            this->local_bind_transform = transform;
        }

        void add_child(Joint child) {
            children.push_back(child);
        }

        glm::mat4 get_animated_transform() {
            return animated_transform;
        }

        void set_animated_transform(glm::mat4 animated_transform) {
            this->animated_transform = animated_transform;
        }

        glm::mat4 get_inverse_bind_tranform() {
            return inverse_bind_transform;
        }

        // Recursively compute inverse of
        void calc_inverse_bind_tranform(glm::mat4 parent_bind_tranform) {
            glm::mat4 bind_transform = parent_bind_tranform * local_bind_transform;
            this->inverse_bind_transform = glm::inverse(bind_transform);
            for (Joint child : this->children) {
                child.calc_inverse_bind_tranform(bind_transform);
            }
        }

        int get_id() {
            return this->id;
        }

        std::vector<Joint> get_children() {
            return this->children;
        }

    private:
        std::vector<Joint> children;
        int id;
        std::string name;
        glm::mat4 local_bind_transform; // The original pose of the model in relation to the parent joint
        glm::mat4 inverse_bind_transform; // The inverse of original pose of the joint in MODEL space (_not_ local space)
        glm::mat4 animated_transform;
};
