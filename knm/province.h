#include "entity.h"

#pragma once

struct provincei : entity {
	short		index;
	void		paint() const;
};
extern provincei* province;

void initialize_province();