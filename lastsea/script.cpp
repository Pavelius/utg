#include "condition.h"
#include "main.h"

extern propertyi::indext prop_visit;

static bool treasure_active(const treasurei* p) {
	return p && p->isactive() && !p->isdiscarded();
}

static bool if_tag(int counter, int param) {
	return game.istag(param + counter);
}

static bool if_story(int counter, int param) {
	return treasure_active(treasurei::pickstory(counter));
}

static bool if_treasure(int counter, int param) {
	return treasure_active(treasurei::pickvaluable(counter));
}

static bool if_visit(int counter, int param) {
	return quest::last && (getnumber(quest::last->index, prop_visit)==counter);
}

void initialize_script() {
	conditioni::add("IfEntry", if_tag, AnswerEntry);
	conditioni::add("IfStory", if_story);
	conditioni::add("IfTreasure", if_treasure);
	conditioni::add("IfVisit", if_visit);
}