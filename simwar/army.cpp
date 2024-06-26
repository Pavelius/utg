#include "army.h"
#include "costitem.h"
#include "player.h"

void add_line(stringbuilder& sb, const costa& source);
int get_value(const char* id, int value);

const army* last_army;

static void army_identifier(stringbuilder& sb, const char* identifier) {
	if(equal(identifier, "Name"))
		sb.add(last_army->getname());
	else if(equal(identifier, "NameOf")) {
		sb.add(last_army->getnameof());
		//if(last_army->hero)
		//	sb.adds(getnm("LeadedBy"), last_army->hero->getname());
	} else if(equal(identifier, "NameOfNoHero"))
		sb.add(last_army->getnameof());
	//else if(equal(identifier, "Units"))
	//	last_army->addunits(sb);
	//else if(equal(identifier, "UnitsAll"))
	//	last_army->addunits(sb, false);
	else if(equal(identifier, "Province"))
		sb.add(last_army->province->getname());
	else if(equal(identifier, "Spoils"))
		add_line(sb, last_army->spoils);
	else if(szstart(identifier, "Total")) {
		auto pv = bsdata<costi>::find(identifier + 5);
		if(pv) {
			auto v = (cost_s)(pv - bsdata<costi>::elements);
			last_army->addtotal(sb, v);
		}
	} else
		stringbuilder::defidentifier(sb, identifier);
}

//static int compare_units(const void* v1, const void* v2) {
//	auto p1 = *((uniti**)v1);
//	auto p2 = *((uniti**)v2);
//	return p2->effect[Strenght] - p1->effect[Strenght];
//}

//void army::addunits(stringbuilder& sb, bool use_distinct) const {
//	collection<uniti> source;
//	for(auto p : *this)
//		source.add(const_cast<uniti*>(p));
//	if(use_distinct)
//		source.distinct();
//	source.sort(compare_units);
//	int count = source.getcount();
//	for(int i = 0; i < count; i++) {
//		if(i > 0) {
//			if(i == count - 1)
//				sb.adds("%-1", getnm("And"));
//			else
//				sb.add(getnm(","));
//		}
//		sb.adds(source[i]->getname());
//	}
//}

void army::act(stringbuilder& sb, const char* format, ...) const {
	auto push_army = last_army;
	auto push_custom = stringbuilder::custom;
	stringbuilder::custom = army_identifier;
	last_army = this;
	sb.addv(format, xva_start(format));
	last_army = push_army;
	stringbuilder::custom = push_custom;
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

void army::addtotal(stringbuilder& sb, cost_s v) const {
	auto push_cost = lastcostitem; costitema it;
	lastcostitem = &it;
	auto total = get(v);
	char temp[512]; stringbuilder sba(temp);
	add_cost_items(sba);
	lastcostitem = push_cost;
	sb.adds("[({"); sb.addv(temp, 0); sb.add("})%1i]", total);
}

int army::geteffect(cost_s v) const {
	switch(v) {
	case Strenght: return strenght;
	case Sword: return casualty;
	default: return 0;
	}
}

int army::get(cost_s v) const {
	int result = geteffect(v);
	if(tactic)
		result += get_value(tactic->id, tactic->effect[v]);
	if(province)
		result += get_value(province->id, province->current[v]);
	return result;
}

int army::get(cost_s v, const army* opponent, cost_s mv) const {
	int result = get(v);
	auto push_cost = lastcostitem;
	lastcostitem = 0; auto value = opponent->get(mv);
	lastcostitem = push_cost;
	if(value > result)
		value = result;
	result += get_value(bsdata<costi>::elements[mv].id, -value);
	return result;
}

void army::select(const provincei* province) {
	units = province->units;
}

void army::select(const provincei* province, const playeri* player) {
	if(province->player==player)
		units = province->units;
}

int	army::getunitcount(const tactici* v) const {
	return 0;
}

void army::match(cost_s v, bool keep) {
}

void army::sort() {
}

void army::normalize() {
}

void army::damage(army& result, int value) {
	if(value > units)
		value = units;
	units -= value;
	casualty += value;
}