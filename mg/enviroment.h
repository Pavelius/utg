#include "nameable.h"

#pragma once

enum enviroment_type_s : unsigned char {
	Wilderness, Village, Towm, City,
};
struct enviroment_typei : nameable {
};
struct enviromenti : nameable {
	enviroment_type_s	type;
};
extern enviromenti* environment;
