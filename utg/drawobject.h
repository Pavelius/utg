#include "color.h"
#include "draw_figure.h"
#include "point.h"

#pragma once

struct sprite;
namespace draw {
extern int			fsize;
struct object : point {
	void*			data;
	const char*		string;
	figure			border;
	color			fore;
	const sprite*	resource;
	unsigned short	frame, size;
	unsigned char	alpha, priority;
	unsigned		flags;
	fnevent			proc;
	object*			linked;
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
void				initializeobjects();
void				paintobjects();
}
