#include "point.h"

#pragma once

// Drawing figure. Inherit this struct to any drawing object and declare metadata to it members.
struct drawable {
	typedef void(*fnpaint)(const drawable* p);
	typedef void(*fnprepare)();
	point			position;
	unsigned char	priority; // priority==0 for empthy object
	explicit operator bool() const { return priority != 0; }
	void			addobject();
	void			clear();
	static void		dowait();
	void			focusing() const;
	static rect		getscreen(int offset);
	static void		input_camera();
	void			paint() const;
	static void		paintall();
	static fnpaint	painting;
	static fnprepare preparing;
	static void		slide(point goal, int step);
	static void		splash(unsigned milliseconds);
};

void setcamera(point v);
extern unsigned long drawable_stamp;

typedef void(*fnevent)();
namespace draw {
bool				isnext();
void				setnext(fnevent v);
}