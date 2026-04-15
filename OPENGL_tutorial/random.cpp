#include "libs.h"

thread_local std::mt19937 rng(std::random_device{}());
std::uniform_real_distribution<float> dist01(0.0f, 1.0f);