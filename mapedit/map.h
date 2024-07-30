#pragma once

template<int X, int Y>
struct mapi {
	static const int mx = X;
	static const int my = Y;
	unsigned char data[mx][my];
	unsigned char operator[](point v) const { return data[v.x][v.y]; }
	unsigned char& operator[](point v) { return data[v.x][v.y]; }
};
