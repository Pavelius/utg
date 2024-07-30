#pragma once

template<int X, int Y, typename T = unsigned char>
struct mapi {
	static const int mx = X;
	static const int my = Y;
	T data[mx][my];
	T operator[](point v) const { return data[v.x][v.y]; }
	T& operator[](point v) { return data[v.x][v.y]; }
};
