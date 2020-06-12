#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include "joint.hpp"
//#include "animator.hpp"
#ifndef MODELINCLUDE
#define MODELINCLUDE
#include "../model.hpp"
#endif

class Animator;

class AnimatedModel : public Model {
    public:
        // Skeleton
        Joint root;
        int joint_count;
        std::vector<glm::vec3> weights;
        std::vector<glm::vec3> joint_ids; // Up to 3 joints can affect each vertex

        // Skin
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        GLuint texture;
        GLuint shader_id;

        Animator *animator;

        //AnimatedModel(char* obj_file, char* texture_file, GLuint shader_id, Animator animator, Joint root, int joint_count) {
        AnimatedModel(char* obj_file, char* texture_file, GLuint shader_id, Joint root, int joint_count, glm::vec3 position) {
            /* Set skin*/
            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uvs;
            std::vector<glm::vec3> _normals;
            // Get vertices, uvs and normals
            loadOBJ(obj_file, _vertices, _uvs, _normals);
            // Set all buffers
            indexVBO(_vertices, _uvs, _normals, this->indices, this->vertices, this->uvs, this->normals);
            /* Set texture file */
            this->texture = load_texture(texture_file, 0);
            /* Set GLSL program reference */
            this->shader_id = shader_id;
            /* Set skeleton */
            this->root = root;
            this->joint_count = joint_count;
            this->model_matrix = glm::translate(glm::mat4(1.0f), position);
            this->root.calc_inverse_bind_tranform(glm::mat4(1.0f));
        }

        void set_animator(Animator *animator) {
            this->animator = animator;
        }

        /*
        void update(float delta_time) {
            animator.update(delta_time);
        }

        void do_animation(Animation *animation) {
            animator.do_animation(animation);
        }
        */

        std::vector<glm::mat4> get_joint_transforms() {
            std::vector<glm::mat4> joint_transforms;
            joint_transforms.resize(this->joint_count);
            add_joints_to_vector(this->root, joint_transforms);
            return joint_transforms;
        }

        void add_joints_to_vector(Joint joint, std::vector<glm::mat4> joint_matrices) {
            joint_matrices[joint.get_id()] = joint.get_animated_transform();
            for (Joint child : joint.get_children()) {
                add_joints_to_vector(child, joint_matrices);
            }
        }
};
