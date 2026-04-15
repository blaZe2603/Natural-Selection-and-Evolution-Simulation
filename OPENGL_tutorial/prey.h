#pragma once
#include "libs.h"
#include "food.h"
//all constants  I might be needing

//for raycasting and vision
const int RADIUS_OF_VISION = 40;
//const float ANGLE_IN_RADIANS = (150.0f / 360.0f) * PI;
const int RADIUS_OF_VISION_SQR = RADIUS_OF_VISION * RADIUS_OF_VISION;

//speed
const float MAXVELOCITY = 1;
const float MINVELOCITY = 0.2;

constexpr float PREY_INITIAL_ENERGY = 3;
constexpr float PREY_REPRODUCE_ENERGY = 6;
constexpr float PREY_INITIAL_VELOCITY = 0.1;
constexpr float PREY_CONST_VELOCITY = 10;
constexpr float PREY_CONST_SENSE = 0.001;
constexpr float PREY_INITIAL_SENSE = 100.0f;

//wandering constants
constexpr float PREY_WANDER_RADIUS = 1;
constexpr float PREY_WANDER_DISTANCE = 4.0f;
constexpr float PREY_WANDER_JITTER = 0.01f;
constexpr float PREY_TURN_CONSTANT = 0.005f;

constexpr float PREY_SPEED_MUTATION = 0.02f;  
constexpr float PREY_SENSE_MUTATION = 5.0f;    

class Prey;
enum PreyState { WANDER, FOOD ,RUN };

void InitPrey();
void CreatePrey(const Prey& parent);
void PreyfindFood(vector<Prey>& prey_array_find, vector<int>& nearest_food_index, vector<Food>& food_array_find);

extern vector<Prey> birth_prey_queue;
extern vector<Prey> prey_array;
extern vector<int> nearest_predator_array;
class Prey {
public:
    float prey_speed;
    float prey_sense;
    float prey_energy;
    float prey_sense_sq;
    glm::mat4 instance_matrices;
    glm::vec2 prey_pos;
    glm::vec2 prey_vel;
    glm::vec2 wander_target;
    float angle;
    glm::vec3 color;
    PreyState state;
    bool alive;

    //constructor
    Prey(glm::vec2 pos, glm::vec2 direction) : prey_pos(pos), prey_speed(PREY_INITIAL_VELOCITY)
    {
        angle = atan2f(direction.y, direction.x);
        prey_energy = PREY_INITIAL_ENERGY;
        prey_vel = direction* PREY_INITIAL_VELOCITY;
        wander_target = glm::vec2(PREY_WANDER_RADIUS, 0.0f);
        instance_matrices = glm::mat4(1.0f);
        prey_sense = PREY_INITIAL_SENSE;
        prey_sense_sq = prey_sense * prey_sense;
        state = WANDER;
        color = glm::vec3(0.0f, 0.0f, 1.0f);
        alive = true;
    }

    void update(float dt)
    {
        prey_sense_sq = prey_sense * prey_sense;
        //update position and check the position to overlap

        prey_pos += prey_vel;
        prey_speed = glm::length(prey_vel);
        if (prey_pos.x < 0) prey_pos.x += WINDOW_WIDTH;
        if (prey_pos.x > WINDOW_WIDTH) prey_pos.x -= WINDOW_WIDTH;
        if (prey_pos.y < 0) prey_pos.y += WINDOW_HEIGHT;
        if (prey_pos.y > WINDOW_HEIGHT) prey_pos.y -= WINDOW_HEIGHT;

        //update energy based on speed and sense
        prey_energy -= ((prey_speed * prey_speed) * PREY_CONST_VELOCITY + (prey_sense)*PREY_CONST_SENSE) * dt;

        if (prey_energy <= 0)
            alive = false;

        if (prey_energy >= PREY_REPRODUCE_ENERGY) 
        {
            prey_energy = 2.0f;
            ::CreatePrey(*this);
        }

        angle = atan2f(prey_vel.y, prey_vel.x);


        //instance matrix
        instance_matrices = glm::mat4(1.0f);
        instance_matrices = glm::translate(instance_matrices, glm::vec3(prey_pos, 0.0f));
        instance_matrices = glm::rotate(instance_matrices, angle, glm::vec3(0, 0, 1));
       
    }
    void wander()
    {
        //using wander algorithm
        wander_target += glm::vec2(
            (rand() / (float)RAND_MAX - 0.5f) * 2.0f * PREY_WANDER_JITTER,
            (rand() / (float)RAND_MAX - 0.5f) * 2.0f * PREY_WANDER_JITTER
        );
        wander_target = glm::normalize(wander_target) * PREY_WANDER_RADIUS;

        glm::vec2 circle_center = glm::normalize(prey_vel) * PREY_WANDER_DISTANCE;
        glm::vec2 target_world = prey_pos + circle_center + wander_target;


        glm::vec2 desired = glm::normalize(target_world - prey_pos) * PREY_INITIAL_VELOCITY;
        glm::vec2 steering = desired - prey_vel;

        float steering_strength = 0.05f;
        prey_vel += steering * steering_strength;
    }
};
    

