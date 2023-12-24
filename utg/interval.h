#pragma once

struct interval {
	unsigned short	min, max;
	constexpr operator bool() const { return min != 0; }
	constexpr int	maximum() const { return max; }
	constexpr int	minimum() const { return min; }
	interval		add(int k, int m = 0) const;
	interval		mul(int k) const;
	int				roll() const;
};
int rollint(int min, int max);
