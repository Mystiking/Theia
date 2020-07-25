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
};

