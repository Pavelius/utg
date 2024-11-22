#include "nameable.h"
#include "variant.h"

#pragma once

struct listi : nameable {
	variants	elements;
	void		run() const;
};

void run_command(const char* id);