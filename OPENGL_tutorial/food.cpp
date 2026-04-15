#include "food.h";

vector<Food> food_array;
vector<int> nearest_food_array;

void InitFood() 
{
    static mt19937 rng(random_device{}());
    static uniform_real_distribution<float> distPosX(0.0f, WINDOW_WIDTH);
    static uniform_real_distribution<float> distPosY(0.0f, WINDOW_HEIGHT);

    food_array.clear();

    for (int i = 0; i < FOOD_COUNT; i++) {
        glm::vec2 pos(distPosX(rng), distPosY(rng));

        food_array.emplace_back(pos);
    }
}
void CreateFood()
{

    mt19937 rng(random_device{}());
    uniform_real_distribution<float> distPosX(0.0f, WINDOW_WIDTH);
    uniform_real_distribution<float> distPosY(0.0f, WINDOW_HEIGHT);
    glm::vec2 pos(distPosX(rng), distPosY(rng));
    food_array.emplace_back(pos);
}

vector<float> CreateCircle(int segments)
{
    vector<float> vertices;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f); 
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; i++) 
    {
        float theta = 2.0f * PI * i / segments;
        vertices.push_back(cos(theta));
        vertices.push_back(sin(theta));
        vertices.push_back(0.0f);
    }
    return vertices;
}