#include "point.h"

#pragma once

extern point objects_mouse;

typedef void(*fnevent)();

struct drawable {
	point			position;
	unsigned char	alpha, priority, param, flags;
	point			getscreen() const;
	bool			isabsolute() const { return priority < 10; }
	void			move(point goal, int speed, int correct = 0, fnevent scene = 0);
};
struct draworder : drawable {
	struct object*	parent;
	drawable		start;
	draworder*		depend;
	unsigned long	tick_start, tick_stop;
	explicit operator bool() const { return parent != 0; }
	void			autoclean() { priority = 0; }
	void			clear();
	void			finish();
	void			setduration(int v) { tick_stop = tick_start + v; }
	void			update();
	void			wait();
};
struct object : drawable {
	const void*		data;
	fnevent			painting;
	constexpr explicit operator bool() const { return painting != 0; }
	draworder*		addorder(int milliseconds = 1000, draworder* depend = 0);
	void			disappear(int milliseconds);
	void			clear();
	void			paint() const;
};

extern object* last_object;

bool cameravisible(point goal, int border = 48);
bool mouseinobjects();

void clear_objects();
void input_camera();
void instant_all();
void focusing(point goal);
void paint_objects();
void setcamera(point v);
void slide_camera(point v, int step = 16);
void splash_screen(unsigned milliseconds);
void show_objects();
void wait_all();

object* addobject(point screen, const void* data, fnevent painting, unsigned char param, unsigned char priority = 21, unsigned char alpha = 0xFF, unsigned char flags = 0);
object* findobject(const void* p);

void* choose_object();

void paint_sprite();

template<class T> void ftpaint() { ((T*)last_object->data)->paint(); }