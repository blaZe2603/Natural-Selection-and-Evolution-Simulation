#pragma once
#include "libs.h"

void InitFood();
void CreateFood();
vector<float> CreateCircle(int segments);
class Food;
extern vector<Food> food_array;
extern vector<int> nearest_food_array;
class Food {
public:
    glm::vec2 food_pos;
    glm::mat4 instanceMatrix;
    glm::vec3 color;
    bool alive;
    Food(glm::vec2 p) {
        food_pos = p;
        instanceMatrix = glm::mat4(1.0f);
        instanceMatrix = glm::translate(instanceMatrix, glm::vec3(food_pos, 0.0f));
        instanceMatrix = glm::scale(instanceMatrix, glm::vec3(FOOD_RADIUS, FOOD_RADIUS, 1.0f));
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        alive = true;
    }
};