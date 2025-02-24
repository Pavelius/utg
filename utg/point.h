#include "rect.h"

#pragma once

struct point {
	union {
		struct {
			short int x, y;
		};
		unsigned int u;
	};
	constexpr point() : x(0), y(0) {}
	constexpr point(int x, int y) : x(x), y(y) {}
	constexpr point(unsigned u) : u(u) {}
	constexpr explicit operator unsigned() const { return u; }
	constexpr explicit operator long() const { return u; }
	constexpr explicit operator bool() const { return u != 0; }
	constexpr bool operator!=(const point pt) const { return pt.x != x || pt.y != y; }
	constexpr bool operator==(const point pt) const { return pt.x == x && pt.y == y; }
	constexpr point operator-(const point pt) const { return{(short)(x - pt.x), (short)(y - pt.y)}; }
	constexpr point operator+(const point pt) const { return{(short)(x + pt.x), (short)(y + pt.y)}; }
	void clear() { u = 0; }
	bool in(const rect& rc) const { return x >= rc.x1 && x <= rc.x2 && y >= rc.y1 && y <= rc.y2; }
	bool in(const point p1, const point p2, const point p3) const;
	int	range(point v) const;
};
long distance(point p1, point p2);