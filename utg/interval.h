#pragma once

struct interval {
	unsigned short	min, max;
	constexpr operator bool() const { return min != 0; }
	constexpr int	maximum() const { return max; }
	constexpr int	minimum() const { return min; }
	int				roll() const;
};
