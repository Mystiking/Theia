#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#ifndef LOADERINCLUDES
#define LOADERINCLUDES
#include "load_texture.h"
#endif
#include <vector>

#ifndef IKINCLUDE
#define IKINCLUDE
#include "IK/builder.hpp"
#endif


class Model {
    public:
        /*
         * Skinning information:
         *  - A ivec3 of bone ids for each vertex in the mesh
         *  - A vec3 of bone weights for each vertex in the mesh
         */
        std::vector<glm::ivec3> bone_ids;
        std::vector<glm::vec3> bone_weights;
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        Skeleton skeleton;

        glm::mat4 model_matrix;

        GLuint texture = 0;
        GLuint shader_id;

        Model() {}

        Model(const char* texture_file) {
            this->texture = load_texture(texture_file, 0);
        }
};

