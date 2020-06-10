#include "drawable.h"
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;
#endif

glm::vec3 get_tile_center_from_position(glm::vec3 position) {
    return glm::vec3(std::ceil(position.x / 2.0f) * 2.0f - 1.0f, 0.0f, std::ceil(position.z / 2.0f) * 2.0f - 1.0f);
}

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

            this->light = glm::vec3(position.x, 10.0f, position.z);
        }

        void set_desired_position(glm::vec3 desired_position) {
            //this->desired_position = desired_position;
            this->desired_position = get_tile_center_from_position(desired_position);
            this->direction = this->desired_position - this->position;
            this->direction /= glm::length(this->direction);
        }

        void move(float delta_time) {
            if (glm::length(desired_position - position) > 1e-4) {
                glm::vec3 position_update = direction * std::min(delta_time * this->speed, glm::length(desired_position - position));
                this->position += position_update;
                this->light    += position_update;
                this->drawable.model_matrix = glm::translate(player_model_matrix, position);
            }
        }

        // Update given time since last update
        void update(float delta_time) {
            std::cout << "player pos=("
                << position.x << ", "
                << position.y << ", "
                << position.z << ")\n";
            move(delta_time);
        }

};

class Enemy {
    public:
        // Player position
        glm::vec3 position;
        // Movement vectors
        glm::vec3 direction;
        glm::vec3 desired_position;
        glm::mat4 enemy_model_matrix;

        float aggro_range = 10.0f;
        float attack_range = 2.0f;

        Drawable drawable;

        Player *player;

        float speed = 5.0f;

        Enemy(glm::vec3 position, Player *player) {
            this->position = position;
            this->player = player;
        }

        Enemy(Player *player) {
            this->position = glm::vec3(0.0f, 0.0f, 0.0f);
            this->player = player;
        }

        void decide_enemy_movement() {
            float player_distance = glm::length(player->position - this->position);
            if ((player_distance < aggro_range) && (player_distance > attack_range)) {
                this->desired_position = get_tile_center_from_position(player->position);
                this->direction = player->position - this->position;
                this->direction /= glm::length(direction);
            } else {
                this->desired_position = this->position;
            }
        }

        void move(float delta_time) {
            if (glm::length(this->desired_position - this->position) > 1e-4) {
                glm::vec3 position_update = this->direction * std::min(delta_time * this->speed, glm::length(desired_position - position));
                this->position += position_update;
                this->drawable.model_matrix = glm::translate(enemy_model_matrix, position);
            }
        }

        // Update given time since last update
        void update(float delta_time) {
            std::cout << "Enemy player pos=("
                << player->position.x << ", "
                << player->position.y << ", "
                << player->position.z << ")\n";
            decide_enemy_movement();
            move(delta_time);
        }

};

Player player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f));

Enemy enemy(glm::vec3(0.0f, 0.0, 0.0), &player);
