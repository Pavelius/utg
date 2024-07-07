#include "nameable.h"

#pragma once

enum class_s : unsigned char {
	NoClass, Fighter, Priest, Theif, Wizard
};
struct classi : nameable {
	int		hits;
};
extern classi* last_class;
