#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
using namespace std;

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>  
#include <gtc/type_ptr.hpp>         



const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

const int PREDATOR_INITIAL_PARTICLES = 2;
const int PREY_INITIAL_PARTICLES = 4;
const int PREY_SIZE = 10;
const int PREDATOR_SIZE = 10;

const float ENERGEY_GAIN = 2;

const int FOOD_SEGMENTS = 30;
const float PI = 3.14159265358979323846f;
const int FOOD_COUNT = 5;
const int FOOD_RADIUS = 7;
const int TIME_PER_FOOD = 1;

extern thread_local std::mt19937 rng;
extern std::uniform_real_distribution<float> dist01;