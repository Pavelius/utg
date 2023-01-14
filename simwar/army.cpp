#include "army.h"
#include "costitem.h"
#include "player.h"

int get_value(const char* id, int value);

struct stringarmy : stringact {
	const army* pa;
	stringarmy(stringbuilder& sb, const army* pa) : stringact(sb, pa->getname(), NoGender), pa(pa) {}
	void addidentifier(const char* identifier) {
		if(equal(identifier, "Name"))
			add(pa->getname());
		else if(equal(identifier, "NameOf")) {
			add(pa->getnameof());
			if(pa->hero)
				adds(getnm("LeadedBy"), pa->hero->getname());
		} else if(szstart(identifier, "Total")) {
			auto pv = bsdata<costi>::find(identifier + 5);
			if(pv) {
				auto v = (cost_s)(pv - bsdata<costi>::elements);
				auto push_cost = lastcostitem; costitema it;
				lastcostitem = &it;
				auto total = pa->get(v);
				char temp[512]; stringbuilder sba(temp);
				add_cost_items(sba);
				lastcostitem = push_cost;
				add("[({"); addv(temp, 0); add("})%1i]", total);
			}
		} else
			stringact::addidentifier(identifier);
	}
};

void army::addunits(stringbuilder& sb) const {
	uniti* last = 0;
	auto need_coma = false;
	for(auto p : *this) {
		if(p == last)
			continue;
		if(need_coma)
			sb.add(",");
		sb.adds(p->getname());
	}
}

void army::act(stringbuilder& sb, const char* format, ...) const {
	stringarmy sa(sb, this);
	sa.addv(format, xva_start(format));
	sb = sa;
}

const char*	army::getname() const {
	if(player)
		return player->getname();
	return getnm("IndependedForce");
}

const char*	army::getnameof() const {
	if(player)
		return getnm(str("%1Of", player->id));
	return getnm("IndependedForceOf");
}

void army::addprompt(stringbuilder& sb) const {
	sb.adds("%-1", getnameof());
	if(hero)
		sb.adds(getnm("LeadedBy"), hero->getname());
}

void army::addtotal(stringbuilder& sb, const char* start, cost_s v) const {
	if(start)
		sb.adds(start);
	auto push_cost = lastcostitem; costitema it;
	lastcostitem = &it;
	auto total = get(v);
	char temp[512]; stringbuilder sba(temp);
	add_cost_items(sba);
	lastcostitem = push_cost;
	sb.adds("[({"); sb.addv(temp, 0); sb.add("})%1i]", total);
}

int army::geteffect(cost_s v) const {
	auto result = 0;
	for(auto p : *this)
		result += get_value(p->id, p->effect[v]);
	return result;
}

int army::get(cost_s v) const {
	int result = geteffect(v);
	if(hero)
		result += get_value(hero->id, hero->effect[v]);
	if(tactic) {
		result += get_value(tactic->id, tactic->effect[v]);
		result += get_value(tactic->id, tactic->perunit[v] * getcount());
	}
	if(province)
		result += get_value(province->id, province->current[v]);
	return result;
}

void army::select(const provincei* province) {
	for(auto& e : bsdata<troop>()) {
		if(e && e.province == province)
			add(e.type);
	}
}

void army::select(const provincei* province, const playeri* player) {
	for(auto& e : bsdata<troop>()) {
		if(e && e.province == province && e.player == player)
			add(e.type);
	}
}

void army::fill(tactica& source) {
	for(auto i = 0; i < 3; i++)
		source.add(bsdata<tactici>::elements + i);
	for(auto p : *this)
		source.add(p->tactics);
	if(hero)
		source.add(hero->tactics);
}

void army::randomtactic() {
}