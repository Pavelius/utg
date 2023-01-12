#include "statable.h"
#include "crt.h"
#include "landscape.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

struct playeri;
struct provincei;

struct provincei : nameable {
	playeri*			player;
	point				position;
	const landscapei*	landscape;
	costac				current, income;
	char				buildings, units, strenght;
	void				add(cost_s v, int value);
	void				clearcurrent();
	void				explore(int value);
	int					get(cost_s v, stringbuilder* psb = 0) const;
	int					getbuildings() const;
	int					getsites() const;
	int					getunits() const;
	int					getunitsvalue(cost_s v) const;
	void				paint() const;
	void				update();
};
struct moveable {
	playeri*			player;
	provincei*			province;
};
struct neighbor {
	short				n1, n2;
};
struct neighbors : adat<provincei*, 16> {
	void				select(const provincei* base);
};

extern provincei*		province;