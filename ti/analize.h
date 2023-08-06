#include "point.h"

#pragma once

const int hms = 8;

class analize {
	short unsigned data[hms * hms];
public:
	void	clear();
	int		getweight(short unsigned i) const;
	void	setweight(short unsigned i, int v);
};

inline point i2h(short unsigned i) { return {(short)(i % hms), (short)(i / hms)}; }
inline short unsigned h2i(point v) { return v.y * hms + v.x; }
