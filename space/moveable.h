#include "drawable.h"

#pragma once

struct moveable : drawable {
	point			start_position, end_position;
	unsigned long	start_tick, end_tick;
	void			domove();
	bool			ismoving() const;
	void			move(point position, unsigned milliseconds);
	void			stop();
};