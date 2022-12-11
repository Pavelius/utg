#include "color.h"
#include "draw_figure.h"
#include "point.h"

#pragma once

struct sprite;
namespace draw {
struct object;
struct drawable : point {
	unsigned char	alpha, priority, mode, flags;
	point			getscreen() const;
};
struct draworder : drawable {
	enum {
		AutoClear, UserFlags
	};
	object*			parent;
	drawable		start;
	draworder*		depend;
	unsigned long	tick_start, tick_stop;
	unsigned		flags;
	explicit operator bool() const { return parent != 0; }
	void			clear();
	bool			is(unsigned v) const { return (flags & (((unsigned)0x80000000) >> v)) != 0; }
	void			set(unsigned v) { flags |= (((unsigned)0x80000000) >> v); }
	void			setduration(int v) { tick_stop = tick_start + v; }
	void			update();
	void			wait();
};
typedef void(*fnpaint)(const object* pointer);
typedef void(*fnupdate)(object* pointer);
struct object : drawable {
	unsigned short	frame, cicle;
	const void*		data;
	static object	def;
	static fnupdate aftercreate;
	static fnpaint	painting;
	draworder*		addorder(int milliseconds = 1000, draworder* depend = 0);
	void			clear();
	static void		initialize();
	void			move(point goal, int speed, int correct = 0);
	void			paint() const;
	void			set(point v) { x = v.x; y = v.y; }
};
object*				addobject(int x, int y);
bool				cameravisible(point goal, int border = 48);
void*				chooseobject();
void				clearobjects();
void				focusing(point goal);
object*				findobject(const void* p);
//const sprite*		getbackground(const char* name);
//const sprite*		getres(const char* name);
//const sprite*		getres(const char* name, const char* folder);
void				paintobjects();
void				setcamera(point v);
void				slidecamera(point v, int step = 16);
void				splashscreen(unsigned milliseconds);
void				showobjects();
void				waitall();
}