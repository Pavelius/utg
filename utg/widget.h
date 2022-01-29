#pragma once

typedef void(*fnevent)();

struct widget {
	const char*		id;
	fnevent			proc, custom;
	static fnevent	last_custom;
	static void		add(const char* id, fnevent proc, fnevent custom = 0);
	static const widget* find(const char* id);
	static void		paint(const char* id);
	explicit operator bool() const { return id != 0; }
};
