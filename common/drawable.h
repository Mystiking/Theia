#include <cmath>
#include <vector>
#include <ostream>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;
#endif
// Common includes
#include "load_texture.h"
#include "objloader.hpp"
#include "vboindexer.hpp"
#include "tangentspace.hpp"

class Drawable {
    public:

        // The quad's FBO. Used only for visualizing the shadowmap.
        GLfloat g_quad_vertex_buffer_data[6*3] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };

        std::vector<unsigned short> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;

        bool has_texture;
        bool buffers_bound;

        GLuint programID, quadProgramID;
        GLuint viewID, modelID, mvpID, lightID;
        GLuint vertexbuffer, uvbuffer, elementbuffer, normalbuffer, colorbuffer, quad_vertexbuffer;
        GLuint texture, renderTexture;
        GLuint textureSamplerID, quadTextureSamplerID;

        glm::mat4 model_matrix;

        Drawable() {
            buffers_bound = false;
            has_texture = false;
        }

        Drawable(glm::mat4 model) {
            buffers_bound = false;
            has_texture = false;
            model_matrix = model;
        }

        void create(
            const char *objpath,
            const char *texturepath,
            // GLSL program ids
            GLuint program_id,
            GLuint quad_program_id,
            // GLSL uniform ids
            GLuint view_id,
            GLuint model_id,
            GLuint mvp_id,
            GLuint light_id,
            // Render texture id
            GLuint render_texture,
            // Texture sample ids
            GLuint texture_sampler_id,
            GLuint quad_texture_sampler_id
        ){
            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uvs;
            std::vector<glm::vec3> _normals;
            // Get vertices, uvs and normals
            loadOBJ(objpath, _vertices, _uvs, _normals);
            // Set all buffers
            indexVBO(_vertices, _uvs, _normals, indices, vertices, uvs, normals);

            // Create texture
            texture = load_texture(texturepath, 1);
            renderTexture = render_texture;
            has_texture = true;

            // Setting GLSL program ids
            programID = program_id;
            quadProgramID = quad_program_id;
            // Setting uniform ids
            viewID = view_id;
            modelID = model_id;
            mvpID = mvp_id;
            lightID = light_id;
            // Setting texture sampling ids
            textureSamplerID = texture_sampler_id;
            quadTextureSamplerID = quad_texture_sampler_id;
        }

        void clean_up() {
            glDeleteBuffers(1, &vertexbuffer);
            glDeleteBuffers(1, &uvbuffer);
            glDeleteBuffers(1, &normalbuffer);
            glDeleteBuffers(1, &elementbuffer);
            glDeleteTextures(1, &texture);
            glDeleteTextures(1, &renderTexture);
        }

        void bind_buffers() {
            if (buffers_bound) {
                return;
            }

            glGenBuffers(1, &vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

            if (has_texture) {
                glGenBuffers(1, &uvbuffer);
                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
                glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
            } else {
                glGenBuffers(1, &colorbuffer);
                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
            }

            glGenBuffers(1, &normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

            glGenBuffers(1, &elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

            glGenBuffers(1, &quad_vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

            buffers_bound = true;
        }

        void draw(glm::mat4 view,
                  glm::mat4 projection,
                  glm::vec3 light
            ) {

            glUseProgram(programID);

            // Compute projection matrices
            glm::mat4 drawable_mvp = projection * view * model_matrix;

            // Send these matrices to the GPU
            glUniformMatrix4fv(mvpID, 1, GL_FALSE, &drawable_mvp[0][0]);
            glUniformMatrix4fv(modelID, 1, GL_FALSE, &model_matrix[0][0]);
            glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);

            glUniform3f(lightID, light.x, light.y, light.z);

            if (has_texture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                glUniform1i(textureSamplerID, 0);
            }

            // 1rst attribute: vertex positions
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            if (has_texture) {
                // 2nd attribute buffer : UVs
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
                glVertexAttribPointer(
                    1,                                // attribute
                    2,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
                );
            } else {
                // 2nd attribute buffer : colors
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glVertexAttribPointer(
                    1,                                // attribute
                    3,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
                );
            }

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glDrawElements(
                GL_TRIANGLES,      // mode
                indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }

        void draw_quads() {
            glUseProgram(quadProgramID);

            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderTexture);
            // Set our "renderedTexture" sampler to use Texture Unit 0
            glUniform1i(quadTextureSamplerID, 0);

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
            glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            glDisableVertexAttribArray(0);

        }

};
