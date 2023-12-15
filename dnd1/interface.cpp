#include "creature.h"
#include "feat.h"
#include "draw_utg.h"

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

static void feats(stringbuilder& sb, const creature* p, const slice<feat_s>& source) {
	for(auto v : source) {
		if(p->is(v))
			sb.adds(bsdata<feati>::elements[v].getname());
	}
}

static void combatant_list() {
	char temp[260]; stringbuilder sb(temp);
	for(auto p : creatures) {
		sb.clear();
		auto ready = p->isready();
		if(!ready)
			sb.add("[~%1]", p->getname());
		else if(p->is(Enemy))
			sb.add("[-%1]", p->getname());
		else
			sb.add(p->getname());
		if(ready) {
			static feat_s source[] = {EngageMelee};
			feats(sb, p, source);
		}
		label(temp, 0, p);
	}
}

void initialize_ui() {
	widget::add("EquipmentList", equipment_list);
	widget::add("BackpackList", backpack_list);
	widget::add("CombatantList", combatant_list);
}