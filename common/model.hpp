#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include "load_texture.h"
//#include "objloader.hpp"
#include "loader.hpp"
#include "vboindexer.hpp"
#include <vector>

class Model {
    public:
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        glm::mat4 model_matrix;

        GLuint texture;
        GLuint shader_id;

        Model() {}

        //Model(const char* obj_file, const char* texture_file, GLuint shader_id) {
        Model(const char* dae_file, const char* texture_file, GLuint shader_id) {
            /* Set skin*/
            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uvs;
            std::vector<glm::vec3> _normals;
            // Get vertices, uvs and normals
            load_dae(dae_file, _vertices, _uvs, _normals);
            // Set all buffers
            indexVBO(_vertices, _uvs, _normals, this->indices, this->vertices, this->uvs, this->normals);
            /* Set texture file */
            this->texture = load_texture(texture_file, shader_id);
            /* Set GLSL program reference */
            this->shader_id = shader_id;
        }

        void update() {
            // Do nothing
        }
};
