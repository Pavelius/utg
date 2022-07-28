#include "nameable.h"
#include "point.h"

#pragma once

class posable : public nameable {
public:
	point		getposition() const;
	void		setposition(point v);
};
