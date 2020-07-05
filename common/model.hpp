#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include "load_texture.h"
#include "objloader.hpp"
#include "dae_loader.hpp"
#include "vboindexer.hpp"
#include <vector>

class Model {
    public:
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::ivec3> joint_ids;
        std::vector<glm::vec3> skinning_weights;

        bool is_animated = false;

        int joint_count = 0;


        glm::mat4 model_matrix;
        std::vector<glm::mat4> local_joint_transforms;

        GLuint texture;
        GLuint shader_id;

        Model() {}

        Model(std::string model_file, const char* texture_file, GLuint shader_id, glm::mat4 model_matrix = glm::mat4(1.0f)) {
            /* Set skin*/
            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uvs;
            std::vector<glm::vec3> _normals;
            // Get vertices, uvs and normals
            if (model_file.substr(model_file.length() - 3, 3) == "dae") {
                // Load model mesh data
                load_mesh_data(model_file.c_str(), _vertices, _uvs, _normals);
                std::vector<glm::ivec3> _joint_ids;
                std::vector<glm::vec3> _skinning_weights;
                // Load model skinning information
                load_skinning_data(model_file.c_str(), _joint_ids, _skinning_weights);
                // Set all buffers
                indexVBO(_vertices, _uvs, _normals, _joint_ids, _skinning_weights,
                         this->indices, this->vertices, this->uvs, this->normals, this->joint_ids, this->skinning_weights);
            } else if (model_file.substr(model_file.length() - 3, 3) == "obj"){
                loadOBJ(model_file.c_str(), _vertices, _uvs, _normals);
                // Set all buffers
                indexVBO(_vertices, _uvs, _normals, this->indices, this->vertices, this->uvs, this->normals);
            } else {
                std::cout << "Unsupported model file type.\n";
                exit(-1);
            }
            /* Set texture file */
            this->texture = load_texture(texture_file, shader_id);
            /* Set GLSL program reference */
            this->shader_id = shader_id;
            /* Set model matrix */
            this->model_matrix = model_matrix;
        }

        std::vector<glm::mat4> get_joint_transforms() {
            return this->local_joint_transforms;
        }

        void update() {
            // Do nothing
        }
};
