#include <cmath>
#include <vector>
#include <ostream>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif
// Common includes
#include "load_texture.h"
#include "objloader.hpp"
#include "vboindexer.hpp"
#include "tangentspace.hpp"

class Drawable {
    public:
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        bool has_texture;
        bool has_normal_texture;
        bool buffers_bound;

        GLuint shader_id, light_id, MVP_id, MV3x3_id, M_id, V_id, P_id, texture_id,
               normal_texture_id;
        GLuint vertexbuffer, uvbuffer, elementbuffer, normalbuffer, tangentbuffer, bitangentbuffer,
               colorbuffer;
        GLuint texture, normal_texture;

        glm::mat4 model_matrix;

        Drawable(glm::mat4 model) {
            buffers_bound = false;
            has_texture = false;
            has_normal_texture = false;

            model_matrix = model;
        }

        void create(
            const char *objpath,
            const char *texturepath,
            const char *normaltexturepath,
            GLuint program,
            GLuint LightID,
            GLuint MVPID,
            GLuint MID,
            GLuint VID,
            GLuint PID,
            GLuint MV3x3ID,
            GLuint textureID,
            GLuint normalTextureID
        ){
            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uvs;
            std::vector<glm::vec3> _normals;
            // Get vertices, uvs and normals
            loadOBJ(objpath, _vertices, _uvs, _normals);
            // Get tangents and bitangents
            std::vector<glm::vec3> _tangents;
            std::vector<glm::vec3> _bitangents;
            computeTangentBasis(_vertices, _uvs, _normals, _tangents, _bitangents);
            // Set all buffers
            indexVBO_TBN(_vertices, _uvs, _normals, _tangents, _bitangents, indices,
                          vertices, uvs, normals, tangents, bitangents);

            // Create textures
            texture = load_texture(texturepath, 1);
            normal_texture = load_texture(normaltexturepath, 1);

            has_texture = true;
            has_normal_texture = true;

            shader_id = program;
            light_id = LightID;
            MVP_id = MVPID;
            M_id = MID;
            V_id = VID;
            P_id = PID;
            MV3x3_id = MV3x3ID;
            texture_id = textureID;
            normal_texture_id = normalTextureID;
        }

        void clean_up() {
            glDeleteBuffers(1, &vertexbuffer);
            glDeleteBuffers(1, &uvbuffer);
            glDeleteBuffers(1, &normalbuffer);
            glDeleteBuffers(1, &tangentbuffer);
            glDeleteBuffers(1, &bitangentbuffer);
            glDeleteBuffers(1, &elementbuffer);
            glDeleteTextures(1, &texture);
            glDeleteTextures(1, &normal_texture);
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

            glGenBuffers(1, &tangentbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
            glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

            glGenBuffers(1, &bitangentbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
            glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

            buffers_bound = true;

        }

        void draw(glm::mat4 view, glm::mat4 projection, glm::vec3 light) {
		    glUseProgram(shader_id);

            // Compute projection matrices
            glm::mat4 drawable_mvp = projection * view * model_matrix;
            glm::mat3 drawable_mv3x3 = glm::mat3(view * model_matrix);
            // Send these matrices to the GPU

            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &drawable_mvp[0][0]);
            glUniformMatrix4fv(M_id, 1, GL_FALSE, &model_matrix[0][0]);
            glUniformMatrix4fv(V_id, 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(P_id, 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix3fv(MV3x3_id, 1, GL_FALSE, &drawable_mv3x3[0][0]);
            glUniform3f(light_id, light[0], light[1], light[2]);

            if (has_texture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                glUniform1i(texture_id, 0);
            }

            if (has_normal_texture) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, normal_texture);
                glUniform1i(normal_texture_id, 1);
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

            // 4th attribute buffer : tangents
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
            glVertexAttribPointer(
                3,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 5th attribute buffer : bitangents
            glEnableVertexAttribArray(4);
            glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
            glVertexAttribPointer(
                4,                                // attribute
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
            glDisableVertexAttribArray(3);
            glDisableVertexAttribArray(4);
        }

};
