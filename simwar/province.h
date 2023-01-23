#include "collection.h"
#include "flagable.h"
#include "landscape.h"
#include "point.h"
#include "statable.h"
#include "stringbuilder.h"
#include "tag.h"

#pragma once

struct playeri;
typedef flagable<32> provincef;

struct provincei : nameable {
	playeri*			player;
	point				position;
	const landscapei*	landscape;
	costac				current, income;
	char				buildings, units, defend, attack, recruit, builded;
	void				add(cost_s v, int value);
	void				explore(int value);
	bool				iswater() const { return landscape->water; }
	bool				isvisible() const;
	void				makewave() const;
	int					get(cost_s v) const { return current[v]; }
	int					getbuildings() const;
	int					getcost() const;
	int					getindex() const { return this - bsdata<provincei>::elements; }
	int					getsites() const;
	int					getunits() const;
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
extern provincef		visibility;
void					clear_wave();
provincei*				find_zero_cost();