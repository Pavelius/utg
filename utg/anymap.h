#include "point.h"

#pragma once

template<class T, unsigned short MPS>
struct anymap {
	typedef T mt;
	static constexpr auto mps = MPS;
	T data[MPS][MPS];
	constexpr T& operator[](point m) { return data[m.y][m.x]; }
	constexpr T operator[](point m) const { return data[m.y][m.x]; }
	T* begin() { return &data[0][0]; }
	const T* end() const { return &data[MPS - 1][MPS - 1] + 1; }
	constexpr T get(point m) const { return data[m.y][m.x]; }
	constexpr T get(short x, short y) const { return data[y][x]; }
	static constexpr bool isvalid(point m) { return ((unsigned short)m.x) < MPS && ((unsigned short)m.y) < MPS; }
};