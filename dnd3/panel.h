#pragma once

#include "nameable.h"
#include "variant.h"

struct paneli : nameable {
	variants	elements;
	int			width, height;
	int			getheight() const;
	int			getwidth() const;
};
extern paneli* last_panel;
