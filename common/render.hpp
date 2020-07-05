#pragma once
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
#include "glm/gtc/quaternion.hpp"
#endif
#ifndef MODELINCLUDE
#define MODELINCLUDE
#include "model.hpp"
#endif
#include "shaders.h"

class Render {
    public:
        GLFWwindow *window;

        std::vector<Model*> models;

        std::vector<GLuint> shaders;

        GLuint VertexArrayID;
        GLuint framebuffer = 0;
        GLuint vertexbuffer, uvbuffer, elementbuffer, normalbuffer, quad_vertexbuffer, jointidbuffer, skinningbuffer;

        GLuint renderTexture;

        int screen_width;
        int screen_height;

        Render(GLFWwindow *window, const char* quad_vertex_shader, const char* quad_fragment_shader, int screen_width, int screen_height) {
            this->window = window;
            this->shaders.push_back(LoadShaders(quad_vertex_shader, quad_fragment_shader));

            this->screen_width = screen_width;
            this->screen_height = screen_height;

            glGenVertexArrays(1, &this->VertexArrayID);
            glBindVertexArray(this->VertexArrayID);

            glGenBuffers(1, &this->quad_vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->quad_vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(this->quad_vertex_buffer_data), this->quad_vertex_buffer_data, GL_STATIC_DRAW);


            glGenFramebuffers(1, &this->framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            glGenTextures(1, &this->renderTexture);
            glBindTexture(GL_TEXTURE_2D, this->renderTexture);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, this->screen_width, this->screen_height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->renderTexture, 0);
            glDrawBuffer(GL_NONE);

        }

        Render(GLFWwindow *window, std::vector<Model*> models, const char* quad_vertex_shader, const char* quad_fragment_shader, int screen_width, int screen_height) {
            this->window = window;

            this->models = models;
            this->shaders.push_back(LoadShaders(quad_vertex_shader, quad_fragment_shader));

            this->screen_width = screen_width;
            this->screen_height = screen_height;


            glGenVertexArrays(1, &this->VertexArrayID);
            glBindVertexArray(this->VertexArrayID);

            glGenFramebuffers(1, &this->framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

            glGenBuffers(1, &this->quad_vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->quad_vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(this->quad_vertex_buffer_data), this->quad_vertex_buffer_data, GL_STATIC_DRAW);

            glGenTextures(1, &this->renderTexture);
            glBindTexture(GL_TEXTURE_2D, this->renderTexture);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, this->screen_width, this->screen_height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->renderTexture, 0);
            glDrawBuffer(GL_NONE);
        }

        void add_shader(const char* vertex_shader, const char* fragment_shader) {
            this->shaders.push_back(LoadShaders(vertex_shader, fragment_shader));
        }

        void add_shader(GLuint shader_id) {
            this->shaders.push_back(shader_id);
        }

        void clean_up() {
            glDeleteBuffers(1, &this->vertexbuffer);
            glDeleteBuffers(1, &this->uvbuffer);
            glDeleteBuffers(1, &this->normalbuffer);
            glDeleteBuffers(1, &this->elementbuffer);
            for (Model* model : this->models) {
                glDeleteTextures(1, &model->texture);
            }
            glDeleteTextures(1, &renderTexture);
            for (GLuint shader_id : this->shaders) {
                glDeleteProgram(shader_id);
            }
            glDeleteVertexArrays(1, &this->VertexArrayID);
        }


        void init(int model_id) {
            glGenBuffers(1, &this->vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->vertices.size() * sizeof(glm::vec3), &this->models[model_id]->vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->uvs.size() * sizeof(glm::vec2), &this->models[model_id]->uvs[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->normals.size() * sizeof(glm::vec3), &this->models[model_id]->normals[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->models[model_id]->indices.size() * sizeof(unsigned short), &this->models[model_id]->indices[0] , GL_STATIC_DRAW);
        }

        void init_animated(int model_id) {
            glGenBuffers(1, &this->vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->vertices.size() * sizeof(glm::vec3), &this->models[model_id]->vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->uvs.size() * sizeof(glm::vec2), &this->models[model_id]->uvs[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->normals.size() * sizeof(glm::vec3), &this->models[model_id]->normals[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->jointidbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->jointidbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->joint_ids.size() * sizeof(glm::ivec3), &this->models[model_id]->joint_ids[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->skinningbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, this->skinningbuffer);
            glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->skinning_weights.size() * sizeof(glm::vec3), &this->models[model_id]->skinning_weights[0], GL_STATIC_DRAW);

            glGenBuffers(1, &this->elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->models[model_id]->indices.size() * sizeof(unsigned short), &this->models[model_id]->indices[0] , GL_STATIC_DRAW);
        }

        void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 light) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0,0,this->screen_width,this->screen_height);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
            // Clear the screen
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Draw everything to the frame buffer
            for (int i = 0; i < (int)this->models.size(); i++) {
                if (this->models[i]->is_animated) {
                    this->draw_animated_model(i, projection, view, light);
                } else {
                    this->draw_model(i, projection, view, light);
                }
            }
            // Draw everything to the screen
            glViewport(0,0,this->screen_width,this->screen_height);
            // Use the quad program shaders
            glUseProgram(this->shaders[0]);
            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->renderTexture);
            // Set our "renderedTexture" sampler to use Texture Unit 0
            GLuint quad_texture_sample_id = glGetUniformLocation(this->shaders[0], "renderedTexture");
            glUniform1i(quad_texture_sample_id, 0);

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, this->quad_vertexbuffer);
            glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            glDisableVertexAttribArray(0);
            // Swap buffers
            glfwSwapBuffers(this->window);
            glfwPollEvents();
        }

        void draw_model(int model_id, glm::mat4 projection, glm::mat4 view, glm::vec3 light) {
            // Bind buffers
            this->init(model_id);
            // Enable the correct GLSL program
            glUseProgram(this->models[model_id]->shader_id);
            // Get handles for transform matrices
            GLuint mvp_matrix_id   = glGetUniformLocation(this->models[model_id]->shader_id, "MVP");
            GLuint view_matrix_id  = glGetUniformLocation(this->models[model_id]->shader_id, "V");
            GLuint model_matrix_id = glGetUniformLocation(this->models[model_id]->shader_id, "M");
            // Get handle for light position
            GLuint light_id = glGetUniformLocation(this->models[model_id]->shader_id, "LightPosition");
            // Get handle for texture sampler
            GLuint texture_sample_id = glGetUniformLocation(this->models[model_id]->shader_id, "TextureSampler");
            // Compute projection matrix
            glm::mat4 mvp = projection * view * this->models[model_id]->model_matrix;
            // Sent the transformation matrices to the shader
            glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, &mvp[0][0]);
            glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &this->models[model_id]->model_matrix[0][0]);
            // Sent light position to the shader
            glUniform3f(light_id, light.x, light.y, light.z);
            // Activate appropriate texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->models[model_id]->texture);
            glUniform1i(texture_sample_id, 0);

            // 1rst attribute: vertex positions
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);
            glDrawElements(
                GL_TRIANGLES,      // mode
                this->models[model_id]->indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);

        }

        void print_mat4(glm::mat4 m) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    std::cout << m[i][j] << " ";
                }
                std::cout << "\n";
            }
        }

        void draw_animated_model(int model_id, glm::mat4 projection, glm::mat4 view, glm::vec3 light) {
            // Bind buffers
            this->init_animated(model_id);
            // Enable the correct GLSL program
            glUseProgram(this->models[model_id]->shader_id);
            // Get handles for transform matrices
            GLuint mvp_matrix_id   = glGetUniformLocation(this->models[model_id]->shader_id, "MVP");
            GLuint view_matrix_id  = glGetUniformLocation(this->models[model_id]->shader_id, "V");
            GLuint model_matrix_id = glGetUniformLocation(this->models[model_id]->shader_id, "M");
            // Get handle for light position
            GLuint light_id = glGetUniformLocation(this->models[model_id]->shader_id, "LightPosition");
            // Get handle for texture sampler
            GLuint texture_sample_id = glGetUniformLocation(this->models[model_id]->shader_id, "TextureSampler");
            // Get handle of joint transforms
            GLuint joint_transforms_id = glGetUniformLocation(this->models[model_id]->shader_id, "jointTransforms");
            // Compute projection matrix
            glm::mat4 mvp = projection * view * this->models[model_id]->model_matrix;
            // Sent the transformation matrices to the shader
            glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, &mvp[0][0]);
            glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &this->models[model_id]->model_matrix[0][0]);
            // Sent light position to the shader
            glUniform3f(light_id, light.x, light.y, light.z);
            // Sent joint transforms to shader
            std::vector<glm::mat4> root_and_child_transform = this->models[model_id]->get_joint_transforms();
            //print_mat4(root_and_child_transform[0]);
            //print_mat4(root_and_child_transform[1]);
            glUniformMatrix4fv(joint_transforms_id, this->models[model_id]->joint_count, GL_FALSE, &root_and_child_transform[0][0][0]);
            //glUniformMatrix4fv(joint_transforms_id, this->models[model_id]->joint_count, GL_FALSE, &ts[0][0][0]);
            // Activate appropriate texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->models[model_id]->texture);
            glUniform1i(texture_sample_id, 0);

            // 1rst attribute: vertex positions
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 4rd attribute buffer : joint indices
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, this->jointidbuffer);
            glVertexAttribIPointer(
                3,                                // attribute
                3,                                // size
                GL_INT,                         // type
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 5th attribute buffer : Skinning weights
            glEnableVertexAttribArray(4);
            glBindBuffer(GL_ARRAY_BUFFER, this->skinningbuffer);
            glVertexAttribPointer(
                4,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);
            glDrawElements(
                GL_TRIANGLES,      // mode
                this->models[model_id]->indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(3);
            glDisableVertexAttribArray(4);

        }

    private:
        // The quad's FBO. Used only for visualizing the shadowmap.
        GLfloat quad_vertex_buffer_data[6*3] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };
};
