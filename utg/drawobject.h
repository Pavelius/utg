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
	void			paint() const;
	void			paintns() const;
	void			setcolorborder();
	void			setcolorform();
	void			setcolorwindow();
};
object*				addobject(int x, int y);
void*				chooseobject();
void				clearobjects();
const sprite*		getres(const char* name);
void				paintobjects();
}
