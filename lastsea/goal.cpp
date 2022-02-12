#include "goal.h"
#include "quest.h"

static int prop_danger_maximum;

goal::fninfo goal::info;

void goal::initialize() {
	prop_danger_maximum = propertyi::add("DangerMaximum", propertyi::Number);
}

const char* goal::getgoalname() const {
	auto p = quest::find(AnswerGoal + value);
	if(!p)
		return "Not found goal";
	auto pv = p->getheader();
	if(!pv)
		return "Goal exist but text empthy";
	return pv;
}

const char* goal::getgoaltext() const {
	auto p = quest::find(AnswerGoal + value);
	if(!p)
		return 0;
	return p->text;
}

int goal::getgoalmaximum() const {
	auto p = quest::find(AnswerGoal + value);
	if(!p)
		return 0;
	return p->getvalue(prop_danger_maximum);
}

void goal::setgoal(int v) {
	if(info)
		info(getnm("AddGoal"), getgoalname());
}