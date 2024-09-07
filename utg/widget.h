#pragma once

typedef void(*fnevent)();

struct widget {
	const char*	id;
	fnevent		proc, click;
	explicit operator bool() const { return id != 0; }
	void		paint() const;
};
extern const widget* last_widget;

void add_widget(const char* id, fnevent proc, fnevent click = 0);