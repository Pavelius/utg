#pragma once

typedef void(*fnevent)();

struct widget {
	const char*	id;
	fnevent		proc, click;
	static const widget* last;
	static void	add(const char* id, fnevent proc, fnevent click = 0);
	static void button(); // External function
	void		paint() const;
	explicit operator bool() const { return id != 0; }
};