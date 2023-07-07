#include "moveable.h"

static int calculate(int v1, int v2, int n, int m) {
	return v1 + (v2 - v1) * n / m;
}

void moveable::stop() {
	start_tick = end_tick = 0;
	end_position = position;
}

bool moveable::ismoving() const {
	return start_tick < end_tick;
}

void moveable::domove() {
	if(start_tick > drawable_stamp)
		return;
	int m = end_tick - start_tick;
	if(m) {
		int n = drawable_stamp - start_tick;
		if(n >= m)
			n = m;
		position.x = (short)calculate(start_position.x, end_position.x, n, m);
		position.y = (short)calculate(start_position.y, end_position.y, n, m);
		if(end_tick <= drawable_stamp)
			stop();
	}
}

void moveable::move(point goal_position, unsigned milliseconds) {
	start_tick = drawable_stamp;
	end_tick = start_tick + milliseconds;
	start_position = position;
	end_position = goal_position;
}