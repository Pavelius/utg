#include "rand.h"

int	last_result, last_roll, last_bonus;

int rollv(int bonus) {
	last_bonus = bonus;
	last_roll = 1 + rand() % 12;
	last_result = last_roll + last_bonus;
	return last_result;
}