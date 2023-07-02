#include "point.h"

#pragma once

// Drawing figure. Inherit this struct to any drawing object and declare metadata to it members.
struct drawable {
	typedef void(*fnpaint)(const drawable* p);
	typedef void(*fnupdate)();
	point			position;
	unsigned char	alpha, priority; // priority==0 for empthy object
	explicit operator bool() const { return priority != 0; }
	void			clear();
	void			focusing() const;
	static rect		getscreen(int offset);
	bool			iswaitable() const;
	void			move(point position, unsigned milliseconds);
	void			paint() const;
	static void		paintall();
	static fnpaint	painting;
	static fnupdate	updating;
	static void		slide(point goal, int step);
	static void		splash(unsigned milliseconds);
	void			wait() const;
	static void		waitall();
};
void setcamera(point v);