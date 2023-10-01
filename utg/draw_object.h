#include "point.h"
#include "rect.h"

#pragma once

extern point objects_mouse;

typedef void(*fnevent)();

struct drawable : point {
	enum { AbsolutePosition, AutoClear };
	unsigned char	alpha, priority, param, flags;
	point			getscreen() const;
	bool			is(int v) const { return (flags & (1 << v)) != 0; }
	void			move(point goal, int speed, int correct = 0);
	void			set(point v) { x = v.x; y = v.y; }
	void			set(int v) { flags |= (1 << v); }
};
struct draworder : drawable {
	struct object*	parent;
	drawable		start;
	draworder*		depend;
	unsigned long	tick_start, tick_stop;
	explicit operator bool() const { return parent != 0; }
	void			clear();
	void			setduration(int v) { tick_stop = tick_start + v; }
	void			update();
	void			wait();
};
struct object : drawable {
	const void*		data;
	unsigned short	frame;
	fnevent			painting;
	draworder*		addorder(int milliseconds = 1000, draworder* depend = 0);
	void			clear();
	void			paint() const;
};

extern object* last_object;

bool cameravisible(point goal, int border = 48);
bool mouseinobjects();

void clear_objects();
void input_camera();
void focusing(point goal);
void paint_objects();
void setcamera(point v);
void slide_camera(point v, int step = 16);
void splash_screen(unsigned milliseconds);
void show_objects();
void wait_all();

object* addobject(point screen, const void* data, fnevent painting, unsigned char priority = 10);
object* findobject(const void* p);

void* choose_object();

template<class T> void ftpaint() { ((T*)last_object->data)->paint(); }