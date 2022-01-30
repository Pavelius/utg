#include "main.h"

static const int StartEntry = 6050;
static const int LastEntry = 6099;

void shiplog::clear() {
	memset(this, 0, sizeof(*this));
	goal_id = 0xFFFF;
}

void shiplog::checkgoal() {
	auto p = getgoal();
	if(!p)
		return;
	if(!p->reach_location || p->reach_location == game.getpage())
		reach_location = true;
}

const char* shiplog::getentryname(int v) {
	if(v > sizeof(tags) * 8 || v < 0)
		return "Not valid entry number";
	auto p = quest::find(v);
	if(!p)
		return "Not found entry";
	if(!p->text)
		return "Entry exist but text empthy";
	return p->text;
}

bool shiplog::istag(int v) const {
	if(v >= StartEntry && v <= LastEntry)
		return tags.is(v - StartEntry);
	return false;
}

void shiplog::settag(int v) {
	if(v >= StartEntry && v <= LastEntry)
		tags.set(v - StartEntry);
}