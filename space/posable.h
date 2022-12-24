#include "nameable.h"
#include "point.h"

#pragma once

class posable : public nameable {
	point		position;
public:
	point		getposition() const { return position; }
	void		setposition(point v);
};
