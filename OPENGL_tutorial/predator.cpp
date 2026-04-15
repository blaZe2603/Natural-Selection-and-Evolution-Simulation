#include "predator.h"
#include "prey.h"
#include "food.h"

vector<Predator> predator_array;
vector<int> nearest_prey_array;
vector<Predator> birth_predator_queue;

void InitPredator() {
    mt19937 rng(random_device{}());
    uniform_real_distribution<float> PredatorAngle(0.0f, 360.0f);
    uniform_real_distribution<float> distPosX(0.0f, WINDOW_WIDTH);
    uniform_real_distribution<float> distPosY(0.0f, WINDOW_HEIGHT);

    predator_array.clear();

    for (int i = 0; i < PREDATOR_INITIAL_PARTICLES; i++) {
        glm::vec2 pos(distPosX(rng), distPosY(rng));

        float angle_deg = PredatorAngle(rng);
        float angle_rad = glm::radians(angle_deg);
        glm::vec2 predator_direction(cos(angle_rad), sin(angle_rad));

        predator_array.emplace_back(pos, predator_direction);
    }
}

void CreatePredator(const Predator& parent)
{
    float angle = (rand() / (float)RAND_MAX) * 2.0f * PI;
    glm::vec2 direction(cos(angle), sin(angle));

    Predator child(parent.predator_pos, direction);

    float speed_mutation =
        ((rand() / (float)RAND_MAX) - 0.5f) * 2.0f * PREDATOR_SPEED_MUTATION;

    float sense_mutation =
        ((rand() / (float)RAND_MAX) - 0.5f) * 2.0f * PREDATOR_SENSE_MUTATION;

    child.predator_speed = parent.predator_speed + speed_mutation;
    child.predator_sense = parent.predator_sense + sense_mutation;

    child.predator_speed = glm::clamp(child.predator_speed, MINVELOCITY, MAXVELOCITY);
    child.predator_sense = glm::max(child.predator_sense, 10.0f);

    child.predator_vel = glm::normalize(direction) * child.predator_speed;

    birth_predator_queue.push_back(child);
}


void PreyfindPredator(vector<Prey>& prey_array_find, vector<int>& nearest_predator_index, vector<Predator>& predator_array_find)
{
    nearest_predator_index.resize(prey_array_find.size());
    float min_dist, dist;
    glm::vec2 diff;

    for (int i = 0; i < prey_array_find.size(); i++)
    {
        min_dist = prey_array_find[i].prey_sense_sq;
        nearest_predator_index[i] = -1;

        for (int j = 0; j < predator_array_find.size(); j++)
        {
            diff = prey_array_find[i].prey_pos - predator_array_find[j].predator_pos;
            dist = glm::dot(diff, diff);

            if (dist < min_dist)
            {
                nearest_predator_index[i] = j;
                min_dist = dist;
            }
        }
    }
}

void PredatorfindPrey(vector<Predator>& predator_array_find, vector<int>& nearest_prey_index, vector<Prey>& prey_array_find)
{
    nearest_prey_index.resize(predator_array_find.size());
    float min_dist, dist;
    glm::vec2 diff;

    for (int i = 0; i < predator_array_find.size(); i++)
    {
        min_dist = predator_array_find[i].predator_sense_sq;
        nearest_prey_index[i] = -1;

        for (int j = 0; j < prey_array_find.size(); j++)
        {
            diff = prey_array_find[j].prey_pos - predator_array_find[i].predator_pos;
            dist = glm::dot(diff, diff);

            if (dist < min_dist)
            {
                nearest_prey_index[i] = j;
                min_dist = dist;
            }
        }
    }
}

