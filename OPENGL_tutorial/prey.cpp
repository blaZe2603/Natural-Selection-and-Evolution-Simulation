    #include "prey.h"
    #include "food.h"
    vector<Prey> prey_array;
    vector<int> nearest_predator_array;
    vector<Prey> birth_prey_queue;
;
    void InitPrey() {
        uniform_real_distribution<float> PreyAngle(0.0f, 360.0f);
        uniform_real_distribution<float> distPosX(0.0f, WINDOW_WIDTH);
        uniform_real_distribution<float> distPosY(0.0f, WINDOW_HEIGHT);

        prey_array.clear();

        for (int i = 0; i < PREY_INITIAL_PARTICLES; i++) {
            glm::vec2 pos(distPosX(rng), distPosY(rng));

            float angle_deg = PreyAngle(rng);
            float angle_rad = glm::radians(angle_deg);
            glm::vec2 prey_direction(cos(angle_rad), sin(angle_rad));

            prey_array.emplace_back(pos, prey_direction);
        }
    }
    void CreatePrey(const Prey& parent)
    {
        float angle = dist01(rng) * 2.0f * PI;
        glm::vec2 direction(cos(angle), sin(angle));

        Prey child(parent.prey_pos, direction);

        float speed_mutation = (dist01(rng) - 0.5f) * 2.0f * PREY_SPEED_MUTATION;
        float sense_mutation = (dist01(rng) - 0.5f) * 2.0f * PREY_SENSE_MUTATION;

        child.prey_speed = parent.prey_speed + speed_mutation;
        child.prey_sense = parent.prey_sense + sense_mutation;

        child.prey_speed = glm::clamp(child.prey_speed, MINVELOCITY, MAXVELOCITY);
        child.prey_sense = glm::max(child.prey_sense, 10.0f);

        child.prey_sense_sq = child.prey_sense * child.prey_sense;

        child.prey_vel = glm::normalize(direction) * child.prey_speed;

        birth_prey_queue.push_back(child);
    }

    void PreyfindFood(vector<Prey>& prey_array_find, vector<int>& nearest_food_index, vector<Food>& food_array_find)
    {
        nearest_food_index.resize(prey_array_find.size());
        float min_dist,dist;
        glm::vec2 diff;

        for (int i = 0; i < prey_array_find.size(); i++)
        {
            min_dist = prey_array_find[i].prey_sense_sq;
            nearest_food_index[i] = -1;

            for (int j = 0; j < food_array_find.size(); j++)
            {
                diff = prey_array_find[i].prey_pos - food_array_find[j].food_pos;
                dist = glm::dot(diff, diff);

                if (dist < min_dist)
                {
                    nearest_food_index[i] = j;
                    min_dist = dist;
                }
            }
        }
    }

