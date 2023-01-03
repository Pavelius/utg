#include "nameable.h"
#include "variant.h"

#pragma once

enum enviroment_type_s : unsigned char {
	Wilderness, Village, Towm, City,
};
struct enviroment_typei : nameable {
};
struct enviromenti : nameable {
	enviroment_type_s	type;
	enviromenti*		parent;
	variants			skills, traits;
};
extern enviromenti* environment;
