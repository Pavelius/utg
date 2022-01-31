#include "main.h"

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
	if(v > AnswerEntry + sizeof(tags) * 8 || v < AnswerEntry)
		return "Not valid entry number";
	auto p = quest::find(v);
	if(!p)
		return "Not found entry";
	if(!p->text)
		return "Entry exist but text empthy";
	return p->text;
}

bool shiplog::istag(int v) const {
	if(v >= AnswerEntry && v < AnswerEntry + 100)
		return tags.is(v - AnswerEntry);
	return false;
}

void shiplog::settag(int v) {
	if(v >= AnswerEntry && v < AnswerEntry + 100)
		tags.set(v - AnswerEntry);
}