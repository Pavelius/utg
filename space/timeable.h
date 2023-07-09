#pragma once

extern unsigned long drawable_stamp;

struct timeable {
	static constexpr unsigned rday = 24 * 60;
	static constexpr unsigned dyear = 12 * 30;
	static unsigned getdays() { return getround() / rday; }
	static unsigned getmonthday() { return 1 + (getdays() % 30); }
	static unsigned getmonth() { return 1 + (getdays() % dyear) / 30; }
	static unsigned getyear() { return getdays() / dyear; }
	static unsigned getround() { return drawable_stamp; }
	void setdate(unsigned year, unsigned month, unsigned day) { drawable_stamp = (rday * (year * dyear + (month - 1) * 30 + (day - 1))); }
	void pass(int v) { drawable_stamp += v; }
};
