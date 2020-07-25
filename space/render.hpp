#pragma once
#include <cmath>
#include <vector>
#include <ostream>
// OpenGL includes
#ifndef GLINCLUDE
#define GLINCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
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
#include "controls.h"

class Render {
    public:
        GLFWwindow *window;

        std::vector<Model*> models;

        // Shaders
        GLuint depth_shader;
        GLuint quad_shader;
        GLuint scene_shader;
        // Buffers
        std::vector<bool> initialized;
        std::vector<GLuint> vaos;
        std::vector<GLuint> vertexbuffers;
        std::vector<GLuint> uvbuffers;
        std::vector<GLuint> normalbuffers;
        std::vector<GLuint> elementbuffers;
        GLuint quad_vao = 0;
        GLuint quad_vbo;
        GLuint depthmapbuffer;
        GLuint depthmaptexture;

        int screen_width;
        int screen_height;
        int shadow_width = 1024;
        int shadow_height = 1024;

        Render(GLFWwindow *window, std::vector<Model*> models, GLuint depth_shader, GLuint quad_shader, GLuint scene_shader, int screen_width, int screen_height) {
            this->window = window;

            this->models = models;

            this->vaos.resize(models.size());
            this->vertexbuffers.resize(models.size());
            this->uvbuffers.resize(models.size());
            this->normalbuffers.resize(models.size());
            this->elementbuffers.resize(models.size());
            for (uint i = 0; i < models.size(); i++)
            {
                this->initialized.push_back(false);
            }


            this->depth_shader = depth_shader;
            this->quad_shader = quad_shader;
            this->scene_shader = scene_shader;

            this->screen_width = screen_width;
            this->screen_height = screen_height;

            // configure depth map FBO
            // -----------------------
            glGenFramebuffers(1, &this->depthmapbuffer);
            // create depth texture
            glGenTextures(1, &this->depthmaptexture);
            glBindTexture(GL_TEXTURE_2D, this->depthmaptexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->shadow_width, this->shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
            // attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, this->depthmapbuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthmaptexture, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void setDepthUniforms(uint model_id) {
            GLuint lightSpaceMatrix_location = glGetUniformLocation(this->depth_shader, "lightSpaceMatrix");
            //GLuint model_location = glGetUniformLocation(this->depth_shader, "model");
            glUniformMatrix4fv(lightSpaceMatrix_location, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
            //glUniformMatrix4fv(model_location, 1, GL_FALSE, &this->models[model_id]->model_matrix[0][0]);
        }

        void setQuadUniforms() {
            GLuint depthMap_location = glGetUniformLocation(this->depth_shader, "depthMap");
            GLuint near_location = glGetUniformLocation(this->depth_shader, "nearPlane");
            GLuint far_location = glGetUniformLocation(this->depth_shader, "farPlane");
            glUniform1i(depthMap_location, 0);
            glUniform1f(near_location, near);
            glUniform1f(far_location, far);
        }

        void setSceneUniforms() {
            GLuint shadowMap_location = glGetUniformLocation(this->scene_shader, "shadowMap");
            GLuint projection_location = glGetUniformLocation(this->scene_shader, "projection");
            GLuint view_location = glGetUniformLocation(this->scene_shader, "view");
            GLuint lightSpaceMatrix_location = glGetUniformLocation(this->scene_shader, "lightSpaceMatrix");
            GLuint viewPosition_location = glGetUniformLocation(this->scene_shader, "viewPosition");
            GLuint lightPosition_location = glGetUniformLocation(this->scene_shader, "lightPosition");
            GLuint near_location = glGetUniformLocation(this->depth_shader, "nearPlane");
            GLuint far_location = glGetUniformLocation(this->depth_shader, "farPlane");

            glUniform1i(shadowMap_location, 0);
            glUniformMatrix4fv(projection_location, 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(lightSpaceMatrix_location, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
            glUniform3fv(viewPosition_location, 1, &camera_position[0]);
            glUniform3fv(lightPosition_location, 1, &lightPosition[0]);
            glUniform1f(near_location, near);
            glUniform1f(far_location, far);
        }

        void setModelUniforms(GLuint shader_id, uint model_id) {
            GLuint model_location = glGetUniformLocation(shader_id, "model");
            glUniformMatrix4fv(model_location, 1, GL_FALSE, &this->models[model_id]->model_matrix[0][0]);
        }

        void renderScene() {
            glClearColor(0.133f, 0.133f, 0.133f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Step 1: Render depth map
            glUseProgram(this->depth_shader);
            glViewport(0, 0, this->shadow_width, this->shadow_height);
            glBindFramebuffer(GL_FRAMEBUFFER, this->depthmapbuffer);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            for (uint model_id = 0; model_id < this->models.size(); model_id++)
            {
                setDepthUniforms(model_id);
                setModelUniforms(this->depth_shader, model_id);
                renderModel(model_id);
            }
            glCullFace(GL_BACK);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Reset viewport
            glViewport(0, 0, this->screen_width, this->screen_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Step 2: Render the scene as normal
            glViewport(0, 0, this->screen_width, this->screen_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(this->scene_shader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->depthmaptexture);
            for (uint model_id = 0; model_id < this->models.size(); model_id++)
            {
                setSceneUniforms();
                setModelUniforms(this->scene_shader, model_id);
                renderModel(model_id);
            }
            
            // (Optional) Step 3: Render depth map to quad for visual debugging
            glUseProgram(this->quad_shader);
            setQuadUniforms();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->depthmaptexture);
            //renderQuad();

            glfwSwapBuffers(this->window);
            glfwPollEvents();
        }

        void renderModel(uint model_id) {
            if (!this->initialized[model_id]) {
                // Initialize buffers
                glGenVertexArrays(1, &this->vaos[model_id]);

                glBindVertexArray(this->vaos[model_id]);
                // Fill buffers
                glGenBuffers(1, &this->vertexbuffers[model_id]);
                glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffers[model_id]);
                glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->vertices.size() * sizeof(glm::vec3), &this->models[model_id]->vertices[0], GL_STATIC_DRAW);

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                );

                glGenBuffers(1, &this->normalbuffers[model_id]);
                glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffers[model_id]);
                glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->normals.size() * sizeof(glm::vec3), &this->models[model_id]->normals[0], GL_STATIC_DRAW);

                // 2nd attribute buffer : normals
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(
                    1,                                // attribute
                    3,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
                );

                glGenBuffers(1, &this->uvbuffers[model_id]);
                glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffers[model_id]);
                glBufferData(GL_ARRAY_BUFFER, this->models[model_id]->uvs.size() * sizeof(glm::vec2), &this->models[model_id]->uvs[0], GL_STATIC_DRAW);

                glEnableVertexAttribArray(2);
                glVertexAttribPointer(
                    2,                  // attribute
                    2,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                );

                glGenBuffers(1, &this->elementbuffers[model_id]);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffers[model_id]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->models[model_id]->indices.size() * sizeof(unsigned short), &this->models[model_id]->indices[0] , GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                this->initialized[model_id] = true;
            }
            // Draw the model
            glBindVertexArray(this->vaos[model_id]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffers[model_id]);
            glDrawElements(
                GL_TRIANGLES,      // mode
                this->models[model_id]->indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
            );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void renderQuad() {
            if (this->quad_vao == 0) {
                float quadVertices[] = {
                    // positions        // texture Coords
                    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                };
                // setup quad VAO
                glGenVertexArrays(1, &this->quad_vao);
                glGenBuffers(1, &this->quad_vbo);
                glBindVertexArray(this->quad_vao);
                glBindBuffer(GL_ARRAY_BUFFER, this->quad_vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            }
            glBindVertexArray(this->quad_vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

};