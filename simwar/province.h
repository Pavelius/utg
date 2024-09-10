#include "collection.h"
#include "entity.h"
#include "flagable.h"
#include "landscape.h"
#include "point.h"
#include "statable.h"
#include "stringbuilder.h"
#include "tag.h"

#pragma once

struct site;

typedef flagable<32> provincef;

struct provincei : entityi {
	playeri*			player;
	point				position;
	const landscapei*	landscape;
	costa				effect;
	char				buildings, recruit, builded, units;
	void				add(costn v, int value);
	void				explore(int value);
	bool				iswater() const { return landscape->water; }
	bool				isvisible() const;
	void				makewave() const;
	int					get(costn v) const { return effect[v]; }
	int					getcost() const;
	int					getindex() const { return this - bsdata<provincei>::elements; }
	int					getsites() const;
	int					getstrenght() const { return get(Strenght) + getunits(); }
	int					getunits() const { return units; }
	void				paint() const;
	void				setblocked() const;
	void				setzerocost() const;
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