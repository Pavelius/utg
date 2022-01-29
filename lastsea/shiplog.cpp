#include "main.h"

void shiplog::clear() {
	memset(this, 0, sizeof(*this));
	goal_id = 0xFFFF;
}

void shiplog::checkgoal() {
	auto p = getgoal();
	if(!p)
		return;
	if(!p->reach_location || p->reach_location == last_page)
		reach_location = true;
}