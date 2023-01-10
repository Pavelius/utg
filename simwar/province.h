#include "statable.h"
#include "crt.h"
#include "landscape.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

struct playeri;

struct provincei : nameable {
	playeri*			owner;
	point				position;
	const landscapei*	landscape;
	costa				income;
	void				add(cost_s v, int value);
	void				explore(int value);
	int					get(cost_s v, stringbuilder* psb = 0) const;
	int					getbuildings() const;
	int					getsites() const;
	int					getunits() const;
	void				paint() const;
};
struct moveable {
	playeri*			player;
	provincei*			province;
};
extern provincei*		province;