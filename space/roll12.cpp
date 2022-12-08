#include "crt.h"

int	rollresult, rollbonus, rolled;

int roll12(int bonus) {
	rolled = 1 + rand() % 12;
	rollbonus = bonus;
	rollresult = rolled + rollbonus;
	return rollresult;
}