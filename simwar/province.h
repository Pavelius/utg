#include "collection.h"
#include "landscape.h"
#include "point.h"
#include "statable.h"
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
	static void			clearwave();
	void				explore(int value);
	bool				iswater() const { return landscape->water; }
	void				makewave() const;
	int					getbuildings() const;
	int					getindex() const { return this - bsdata<provincei>::elements; }
	int					getsites() const;
	int					getunits() const;
	int					getunitsvalue(cost_s v) const;
	void				paint() const;
	void				setblocked() const;
	void				setzerocost() const;
	void				update();
};
struct moveable {
	playeri*			player;
	provincei*			province;
};
struct neighbor {
	short				n1, n2;
};
struct neightbors : collection<provincei> {
	void				selectn(const provincei* base);
};
extern provincei*		province;
void					clear_wave();