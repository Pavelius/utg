#include "main.h"

using namespace draw;

static void paint_items(creature* p, bool equipment) {
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : p->wears) {
		if(!e)
			continue;
		auto slot = (wear_s)(&e - p->wears);
		if((slot <= BackpackLast) == equipment)
			continue;
		sb.clear(); e.addname(sb);
		label(temp, 0, &e);
	}
}

static void equipment_list() {
	if(bsdata<creature>::have(focus_object))
		paint_items((creature*)focus_object, true);
}

static void backpack_list() {
	if(bsdata<creature>::have(focus_object))
		paint_items((creature*)focus_object, false);
}

static void combatant_list() {
	for(auto p : game.creatures)
		label(p->getname(), 0, p);
}

void initialize_ui() {
	widget::add("EquipmentList", equipment_list);
	widget::add("BackpackList", backpack_list);
	widget::add("CombatantList", combatant_list);
}