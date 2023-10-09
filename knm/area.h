#include "anymap.h"

#pragma once

enum direction_s : unsigned char;

struct areai : anymap<unsigned char, 8> {
	static void			blockrange(int range);
	static void			blockzero();
	void				clear();
	static void			clearpath();
	static direction_s	getdirection(point s, point d);
	point				getnext(point start, point goal) const;
	point				getnext(point start, direction_s d) const;
	unsigned			getpath(point start, point goal, point* result, unsigned maximum) const;
	bool				ishostile(point pt);
	static void			makewave(point start_index);
	static void			makewavex();
	void				set(point m, mt v);
	static void			setblock(point m, mt v);
};
extern areai area;