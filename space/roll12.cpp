#include "crt.h"

int	rollresult, rollbonus, rolled;

void roll12(int bonus) {
	rolled = 1 + rand() % 12;
	rollbonus = bonus;
	rollresult = rolled + rollbonus;
}