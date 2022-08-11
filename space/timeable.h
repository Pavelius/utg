#pragma once

class timeable {
	const unsigned rday = 24 * 60;
	const unsigned dyear = 12 * 30;
	unsigned		rounds; // 1 round is 1 minute
public:
	unsigned		getdays() const { return rounds / rday; }
	unsigned		getmonthday() const { return 1 + (getdays() % 30); }
	unsigned		getmonth() const { return 1 + (getdays() % dyear) / 30; }
	unsigned		getyear() const { return getdays() / dyear; }
	unsigned		getround() const { return rounds; }
	void			setdate(unsigned year, unsigned month, unsigned day) { rounds = rday * (year * dyear + (month - 1) * 30 + (day - 1)); }
	void			pass(int v) { rounds += v; }
};
