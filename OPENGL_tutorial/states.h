#pragma once
#include "predator.h"
#include "prey.h"
#include "food.h"

void PreyUpdate(vector<Prey>& prey_array_find, vector<Food>& food_array_find, vector<Predator>& predator_array_find, vector<int>& nearest_food_index, vector<int>& nearest_predator_index);
void PreyFood(Prey& prey, Food& food);
void PreyPredator(Prey& prey, Predator& predator);

void PredatorUpdate(vector<Prey>& prey_array_find, vector<Predator>& predator_array_find, vector<int>& nearest_prey_index);
void PredatorPrey(Predator& predator,Prey& prey);