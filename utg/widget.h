#pragma once

typedef void(*fnevent)();

struct widget {
	const char*		id;
	fnevent			proc;
	static const widget* find(const char* id);
	static void		paint(const char* id);
	explicit operator bool() const { return id != 0; }
};
