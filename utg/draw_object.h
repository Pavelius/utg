#pragma once
#include "point.h"

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
	void			autoclear() { priority = 250; }
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
};

extern object* last_object;

bool cameravisible(point goal, int border = 48);
void clear_objects();
void focusing(point goal);
void input_camera();
void instant_all();
bool mouseinobjects();
void normalize_objects();
void paint_objects();
void paint_sprite();
void setcamera(point v);
void shrink_objects();
void slide_camera(point v, int step = 16);
void splash_screen(unsigned milliseconds, fnevent proc = 0);
void show_objects();
void wait_all();

object* addobject(point screen, const void* data, fnevent painting, unsigned char param, unsigned char priority = 21, unsigned char alpha = 0xFF, unsigned char flags = 0);
object* findobject(const void* p);
object* findobject(point position, fnevent proc);

void* choose_object();

template<typename T> void ftpaint() { ((T*)last_object->data)->paint(); }
template<typename T> void updateui(T* p);