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
	object*			parent;
	drawable		start;
	unsigned long	tick;
	void			clear();
	void			update();
};
struct object : drawable {
	enum {
		Visible, Hilite,
	};
	void*			data;
	const char*		string;
	figure			shape;
	const sprite*	resource;
	unsigned short	frame, size;
	unsigned char	priority;
	unsigned		flags;
	fnevent			proc;
	static object	def;
	draworder*		addorder();
	void			clear();
	static void		initialize();
	bool			is(unsigned v) const { return (flags & (((unsigned)0x80000000)>>v)) != 0; }
	void			move(point goal, int speed);
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
void				splashscreen(unsigned milliseconds);
void				showobjects();
}
