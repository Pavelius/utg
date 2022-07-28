#pragma once

class timeable {
	int				rounds; // 1 round is 1 minute
public:
	int				getdays() const { return rounds / (24 * 60); }
	int				getmonthday() const { return (getdays() % 30); }
	int				getmonth() const { return (getdays() % (12 * 30)) / 30; }
	int				getyear() const { return getdays() / (12 * 30); }
	int				getround() const { return rounds; }
	void			pass(int v) { rounds += v; }
};
