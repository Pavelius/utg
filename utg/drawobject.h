#include "color.h"
#include "draw_figure.h"
#include "point.h"

#pragma once

typedef void(*fnevent)();
struct sprite;

namespace draw {
extern point		camera;
extern int			fsize;
struct object : point {
	enum {
		Visible, Hilite,
	};
	void*			data;
	const char*		string;
	figure			shape;
	color			fore;
	const sprite*	resource;
	unsigned short	frame, size;
	unsigned char	alpha, priority;
	unsigned		flags;
	fnevent			proc;
	static object	def;
	void			clear();
	static void		initialize();
	bool			is(unsigned v) const { return (flags & (((unsigned)0x80000000)>>v)) != 0; }
	void			paint() const;
	void			paintns() const;
	void			set(unsigned v) { flags |= (((unsigned)0x80000000) >> v); }
	void			setcolorborder();
	void			setcolorform();
	void			setcolorwindow();
};
object*				addobject(int x, int y);
void*				chooseobject();
void				clearobjects();
const sprite*		getres(const char* name);
void				paintobjects();
void				splashscreen(unsigned milliseconds);
}
