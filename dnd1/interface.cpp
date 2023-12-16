#include "creature.h"
#include "feat.h"
#include "draw_utg.h"

using namespace draw;

static const char* start_header;

static void add_header(stringbuilder& sb, const char* separator) {
	if(start_header[0] == 0)
		sb.addv(separator, 0);
}

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

static void feats(stringbuilder& sb, const creature* p, const variants& source) {
	start_header = sb.get();
	for(auto v : source) {
		if(v.iskind<feati>()) {
			if(p->feats.is(v.value) == (v.counter >= 0)) {
				add_header(sb, " - ");
				sb.adds("%-1", bsdata<feati>::elements[v.value].getname());
			}
		}
	}
}

static void combatant_list() {
	char temp[260]; stringbuilder sb(temp);
	auto pcol = bsdata<listi>::find("CombatantStatuses");
	for(auto p : creatures) {
		sb.clear();
		auto ready = p->isready();
		bool close_scope = false;
		if(!ready) {
			sb.add("[~");
			close_scope = true;
		} else if(p->is(Enemy)) {
			sb.add("[-");
			close_scope = true;
		}
		sb.add(p->getname());
		if(pcol)
			feats(sb, p, pcol->elements);
		if(close_scope)
			sb.add("]");
		label(temp, 0, p);
	}
}

void initialize_ui() {
	widget::add("EquipmentList", equipment_list);
	widget::add("BackpackList", backpack_list);
	widget::add("CombatantList", combatant_list);
}