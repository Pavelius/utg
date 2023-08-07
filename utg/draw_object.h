#include "point.h"
#include "rect.h"

#pragma once

extern point objects_mouse;

typedef void(*fnevent)();

namespace draw {
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
	static object	def;
	draworder*		addorder(int milliseconds = 1000, draworder* depend = 0);
	void			clear();
	static void		initialize();
	void			paint() const;
};

extern object* last_object;

bool cameravisible(point goal, int border = 48);
bool mouseinobjects();

void clearobjects();
void inputcamera();
void focusing(point goal);
void paint_objects();
void setcamera(point v);
void slidecamera(point v, int step = 16);
void splashscreen(unsigned milliseconds);
void showobjects();
void waitall();

object* addobject(point screen, void* data, fnevent painting);
object* findobject(const void* p);

void* chooseobject();

//template<class T> void ftpaint() { ((T*)last_object->data)->paint(); }

}