#pragma once

typedef void(*fnevent)();

struct widget {
	const char*		id;
	fnevent			proc, click;
	explicit operator bool() const { return id != 0; }
	static void		add(const char* id, fnevent proc, fnevent click = 0);
	static void		button(); // External function
	void			paint() const;
};
extern const widget* lastwidget;