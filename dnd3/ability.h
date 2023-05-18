#include "nameable.h"
#include "variant.h"

#pragma once

struct abilityi : nameable {
	variants	formula;
};
struct statable {
	char		abilities[64];
	void		update();
};
extern int ability_result;
extern int result_param[4];