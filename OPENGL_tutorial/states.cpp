#include "predator.h";
#include "prey.h"
#include "food.h"
#include "states.h"

void PreyUpdate(vector<Prey>& prey_array_find, vector<Food>& food_array_find, vector<Predator>& predator_array_find, vector<int>& nearest_food_index, vector<int>& nearest_predator_index)
{
	for (int i = 0;i <prey_array_find.size();i++)
	{
		Prey& prey = prey_array_find[i];
		if (nearest_predator_index[i] != -1)
		{
			prey.state = RUN;
			PreyPredator(prey, predator_array_find[nearest_predator_index[i]]);
		}
		else if (nearest_food_index[i] != -1)
		{
			prey.state = FOOD;
			PreyFood(prey, food_array_find[nearest_food_index[i]]);
		}
		else
		{
			prey.state = WANDER;
			prey.wander();
		}
		switch (prey.state)
		{
		case RUN:
			prey.color = glm::vec3(1.0f, 0.0f, 0.0f); 
			break;

		case WANDER:
			prey.color = glm::vec3(0.0f, 0.0f, 1.0f); 
			break;
		case FOOD:
			prey.color = glm::vec3(0.0f, 1.0f, 0.0f); 
			break;
		}
	}

}

void PredatorUpdate(vector<Prey>& prey_array_find, vector<Predator>& predator_array_find, vector<int>& nearest_prey_index)
{
	for (int i = 0;i < predator_array_find.size();i++)
	{
		Predator& predator = predator_array_find[i];
		if (nearest_prey_index[i] != -1)
		{
			predator.state = CHASING;
			PredatorPrey(predator, prey_array_find[nearest_prey_index[i]]);
		}
		else
		{
			predator.state = WANDERING;
			predator.wander();
		}
		switch (predator.state)
		{
		case CHASING:
			predator.color = glm::vec3(1.0f, 0.0f, 0.0f); 
			break;

		case WANDERING:
			predator.color = glm::vec3(0.0f, 0.0f, 1.0f); 
			break;
		}
	}


}


void PreyFood(Prey& prey, Food& food)
{
	glm::vec2 dir = food.food_pos - prey.prey_pos;
	if (glm::length(dir) < 5 && food.alive)
	{
		food.alive = false;
		prey.prey_energy += ENERGEY_GAIN;
	}

	dir = glm::normalize(dir);
	glm::vec2 curr_dir = glm::vec2(cos(prey.angle), sin(prey.angle));
	glm::vec2 final_dir = curr_dir + (dir - curr_dir) * PREY_TURN_CONSTANT;
	final_dir = glm::normalize(final_dir);
	prey.prey_vel = final_dir * prey.prey_speed;
}

void PreyPredator(Prey& prey, Predator& predator)
{
	glm::vec2 dir = prey.prey_pos - predator.predator_pos;

	dir = glm::normalize(dir);
	glm::vec2 curr_dir = glm::vec2(cos(prey.angle), sin(prey.angle));
	glm::vec2 final_dir = curr_dir + (dir - curr_dir) * PREY_TURN_CONSTANT;

	final_dir = glm::normalize(final_dir);
	prey.prey_vel = final_dir * prey.prey_speed;
}

void PredatorPrey(Predator& predator, Prey& prey)
{
	glm::vec2 dir = prey.prey_pos - predator.predator_pos;

	if (glm::length(dir) < 5 && prey.alive)
	{
		prey.alive = false;
		predator.predator_energy += ENERGEY_GAIN;
	}

	dir = glm::normalize(dir);
	glm::vec2 curr_dir = glm::vec2(cos(predator.angle), sin(predator.angle));
	glm::vec2 final_dir = curr_dir + (dir - curr_dir) * PREDATOR_TURN_CONSTANT;

	final_dir = glm::normalize(final_dir);
	predator.predator_vel = final_dir * predator.predator_speed;
}

