#include "color.h"
#include "draw_figure.h"
#include "point.h"

#pragma once

typedef void(*fnevent)();
struct sprite;

namespace colors {
extern color		button, form, window;
extern color		border, active;
extern color		text, h1, h2, h3, special;
namespace tips {
extern color		back;
extern color		text;
}
}
namespace draw {
extern point		camera;
extern int			fsize;
struct object;
struct drawable : point {
	unsigned char	alpha;
	color			fore;
};
struct draworder : drawable {
	enum {
		AutoClear,
	};
	object*			parent;
	drawable		start;
	draworder*		depend;
	unsigned long	tick_start, tick_stop;
	unsigned		flags;
	explicit operator bool() const { return parent != 0; }
	static int		calculate(int v1, int v2, int n, int m);
	void			clear();
	bool			is(unsigned v) const { return (flags & (((unsigned)0x80000000) >> v)) != 0; }
	void			set(unsigned v) { flags |= (((unsigned)0x80000000) >> v); }
	void			setduration(int v) { tick_stop = tick_start + v; }
	void			update();
	void			wait();
};
struct object : drawable {
	enum {
		Visible, Hilite,
	};
	const void*		data;
	const char*		string;
	figure			shape;
	const sprite*	resource;
	const sprite*	font;
	unsigned short	frame, size;
	unsigned char	priority;
	unsigned		flags;
	fnevent			proc;
	static object	def;
	draworder*		addorder(int milliseconds = 1000, draworder* depend = 0);
	void			clear();
	static void		initialize();
	bool			is(unsigned v) const { return (flags & (((unsigned)0x80000000)>>v)) != 0; }
	void			move(point goal, int speed, int correct = 0);
	void			paint() const;
	void			paintns() const;
	void			set(unsigned v) { flags |= (((unsigned)0x80000000) >> v); }
	void			set(point v) { x = v.x; y = v.y; }
};
object*				addobject(int x, int y);
void*				chooseobject();
void				clearobjects();
object*				findobject(const void* p);
const sprite*		getbackground(const char* name);
const sprite*		getres(const char* name);
const sprite*		getres(const char* name, const char* folder);
void				paintobjects();
void				setcamera(point v);
void				slidecamera(point v, int step = 8);
void				splashscreen(unsigned milliseconds);
void				showobjects();
void				waitall();
}