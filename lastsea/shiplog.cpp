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
	auto pv = p->getheader();
	if(!pv)
		return "Entry exist but text empthy";
	return pv;
}

const char* shiplog::getentrytext(int v) {
	auto p = getentry(v);
	if(!p)
		return 0;
	return p->text;
}

const quest* shiplog::getentry(int v) {
	if(v >= AnswerEntry && v < AnswerEntry + sizeof(tags) * 8)
		return quest::find(v);
	return 0;
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