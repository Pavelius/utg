#pragma once

extern unsigned long drawable_stamp;

struct timeable {
	static constexpr unsigned rday = 24 * 60;
	static constexpr unsigned dyear = 12 * 30;
	unsigned getdays() const { return getround() / rday; }
	unsigned getmonthday() const { return 1 + (getdays() % 30); }
	unsigned getmonth() const { return 1 + (getdays() % dyear) / 30; }
	unsigned getyear() const { return getdays() / dyear; }
	unsigned getround() const { return drawable_stamp; }
	void setdate(unsigned year, unsigned month, unsigned day) { drawable_stamp = (rday * (year * dyear + (month - 1) * 30 + (day - 1))); }
	void pass(int v) { drawable_stamp += v; }
};
