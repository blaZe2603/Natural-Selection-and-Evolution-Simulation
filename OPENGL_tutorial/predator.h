#pragma once
#include "prey.h"
#include "food.h"

//all constants  I might be needing

constexpr float PREDATOR_INITIAL_ENERGY = 3;
constexpr float PREDATOR_INITIAL_VELOCITY = 0.12;
constexpr float PREDATOR_CONST_VELOCITY = 10;
constexpr float PREDATOR_CONST_SENSE = 0.001;
constexpr float PREDATOR_REPRODUCE_ENERGY = 6;

//wandering constants
constexpr float PREDATOR_WANDER_RADIUS = 1;
constexpr float PREDATOR_WANDER_DISTANCE = 4.0f;
constexpr float PREDATOR_WANDER_JITTER = 0.01f;
constexpr float PREDATOR_TURN_CONSTANT = 0.005f;
//mutation constants

constexpr float PREDATOR_SPEED_MUTATION = 0.02f;
constexpr float PREDATOR_SENSE_MUTATION = 5.0f;

class Predator;
void InitPredator();
void CreatePredator(const Predator& parent);
void PreyfindPredator(vector<Prey>& prey_array_find, vector<int>& nearest_predator_index, vector<Predator>& predator_array_find);
void PredatorfindPrey(vector<Predator>& predator_array_find, vector<int>& nearest_prey_index, vector<Prey>& prey_array_find);

extern vector<Predator> predator_array;
extern vector<int> nearest_prey_array;
extern vector<Predator> birth_predator_queue;
enum PredatorState { WANDERING, CHASING };
class Predator {
public:

    float predator_speed;
    float predator_sense;
    float predator_energy;
    float predator_sense_sq;
    glm::mat4 instance_matrices;
    glm::vec2 predator_pos;
    glm::vec2 predator_vel;
    glm::vec2 wander_target;
    float angle;
    glm::vec3 color;
    PredatorState state;
    bool alive;
    //constructor
    Predator(glm::vec2 pos, glm::vec2 direction)
        : predator_pos(pos), predator_speed(PREDATOR_INITIAL_VELOCITY)
    {
        angle = atan2f(direction.y, direction.x);
        predator_vel = direction * PREDATOR_INITIAL_VELOCITY;
        predator_energy = PREDATOR_INITIAL_ENERGY;
        wander_target = glm::vec2(PREDATOR_WANDER_RADIUS, 0.0f);
        instance_matrices = glm::mat4(1.0f);
        predator_sense = 100.0f;
        predator_sense_sq = predator_sense * predator_sense;
        state = WANDERING;
        color = glm::vec3(0.0f, 0.0f, 1.0f);
        alive = true;
    }

    void update(float dt) 
    {

        predator_sense_sq = predator_sense * predator_sense;
        //update position and check the position to overlap
        predator_pos += predator_vel;
        if (predator_pos.x < 0) predator_pos.x += WINDOW_WIDTH;
        if (predator_pos.x > WINDOW_WIDTH) predator_pos.x -= WINDOW_WIDTH;
        if (predator_pos.y < 0) predator_pos.y += WINDOW_HEIGHT;
        if (predator_pos.y > WINDOW_HEIGHT) predator_pos.y -= WINDOW_HEIGHT;

        predator_energy -= ((predator_speed * predator_speed) * PREDATOR_CONST_VELOCITY + (predator_sense)*PREDATOR_CONST_SENSE)*dt;
        
        if (predator_energy <= 0)
            alive = false;
        if (predator_energy >= PREDATOR_REPRODUCE_ENERGY)
        {
            predator_energy = 2.0f;
            ::CreatePredator(*this);
        }
        angle = atan2f(predator_vel.y, predator_vel.x);

        //instance matrix
        instance_matrices = glm::mat4(1.0f);
        instance_matrices = glm::translate(instance_matrices, glm::vec3(predator_pos, 0.0f));
        instance_matrices = glm::rotate(instance_matrices, angle, glm::vec3(0, 0, 1));
    }
    void wander()
    {
        wander_target += glm::vec2(
            (rand() / (float)RAND_MAX - 0.5f) * 2.0f * PREDATOR_WANDER_JITTER,
            (rand() / (float)RAND_MAX - 0.5f) * 2.0f * PREDATOR_WANDER_JITTER
        );
        wander_target = glm::normalize(wander_target) * PREDATOR_WANDER_RADIUS;

        glm::vec2 circle_center = glm::normalize(predator_vel) * PREDATOR_WANDER_DISTANCE;
        glm::vec2 target_world = predator_pos + circle_center + wander_target;

        glm::vec2 desired = glm::normalize(target_world - predator_pos) * predator_speed;
        glm::vec2 steering = desired - predator_vel;

        predator_vel += steering * 0.05f;
        predator_vel = glm::normalize(predator_vel) * predator_speed;

    }
};