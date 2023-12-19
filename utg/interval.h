#pragma once

struct interval {
	unsigned short	min, max;
	constexpr int	maximum() const { return max; }
	constexpr int	minimum() const { return min; }
	int				roll() const;
};
