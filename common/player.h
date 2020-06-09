#include "drawable.h"
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;
#endif

class Player {
    public:
        // Player position
        glm::vec3 position;
        // Player light position
        glm::vec3 light;
        // Movement vectors
        glm::vec3 desired_position;
        glm::vec3 direction;
        glm::mat4 player_model_matrix;
        Drawable drawable;

        float speed = 10.0f;

        Player(glm::vec3 position, glm::vec3 player_light) {
            this->position = position;
            this->desired_position = position;

            this->light = player_light;
        }

        Player() {
            this->position = glm::vec3(0.0f, 0.0f, 0.0f);
            this->desired_position = glm::vec3(0.0f, 0.0f, 0.0f);

            this->light = glm::vec3(position.x, 10, position.z);
        }

        void move(glm::vec3 desired_position) {
            this->desired_position = desired_position;
            this->direction = this->desired_position - this->position;
            this->direction /= glm::length(this->direction);
        }

        // Update given time since last update
        void update(float delta_time) {
            if (glm::length(desired_position - position) > 1e-4) {
                glm::vec3 position_update = direction * std::min(delta_time * this->speed,
                                                                 glm::length(desired_position - position));
                this->position += position_update;
                this->light    += position_update;
                this->drawable.model_matrix = glm::translate(player_model_matrix, position);
            }
        }

};

Player player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f));
