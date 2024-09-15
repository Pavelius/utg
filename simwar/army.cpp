#include "army.h"
#include "costitem.h"
#include "moveorder.h"
#include "player.h"

void add_line(stringbuilder& sb, const costa& source);
int get_value(const char* id, int value);

const army* last_army;

static void army_identifier(stringbuilder& sb, const char* identifier) {
	if(equal(identifier, "Name"))
		sb.add(last_army->getname());
	else if(equal(identifier, "NameOf"))
		sb.add(last_army->getnameof());
	else if(equal(identifier, "NameOfNoHero"))
		sb.add(last_army->getnameof());
	else if(equal(identifier, "Province"))
		sb.add(last_army->province->getname());
	else if(equal(identifier, "Spoils"))
		add_line(sb, last_army->result);
	else if(szstart(identifier, "Total")) {
		auto pv = bsdata<costi>::find(identifier + 5);
		if(pv) {
			auto v = (costn)(pv - bsdata<costi>::elements);
			last_army->addtotal(sb, v);
		}
	} else
		default_string(sb, identifier);
}

static void act_army(stringbuilder& sb, const army* pa, const char* format, const char* format_param, char separator) {
	if(!format || !format[0])
		return;
	auto push_army = last_army;
	last_army = pa;
	if(separator)
		sb.addsep(separator);
	auto push_custom = stringbuilder::custom;
	stringbuilder::custom = army_identifier;
	sb.addv(format, xva_start(format));
	stringbuilder::custom = push_custom;
	last_army = push_army;
}

void army::act(stringbuilder& sb, const char* format, ...) const {
	act_army(sb, this, format, xva_start(format), ' ');
}

void army::actn(stringbuilder& sb, const char* format, ...) const {
	act_army(sb, this, format, xva_start(format), '\n');
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
}

int army::get(costn v) const {
	switch(v) {
	case Strenght: return result[Strenght] + units;
	default: return result[v];
	}
}

void army::addtotal(stringbuilder& sb, costn v) const {
	auto push_cost = lastcostitem; costitema it;
	lastcostitem = &it;
	auto total = get(v);
	char temp[512]; stringbuilder sba(temp);
	add_cost_items(sba);
	lastcostitem = push_cost;
	sb.adds("[({"); sb.addv(temp, 0); sb.add("})%1i]", total);
}

int army::get(costn v, const army* opponent, costn mv) const {
	int result = get(v);
	auto push_cost = lastcostitem;
	lastcostitem = 0; auto value = opponent->get(mv);
	lastcostitem = push_cost;
	if(value > result)
		value = result;
	result += get_value(bsdata<costi>::elements[mv].id, -value);
	return result;
}

void army::defenders() {
	units = province->getunits();
}

void army::attackers() {
	units = 0;
	for(auto& e : bsdata<moveorder>()) {
		if(e.player == player && e.getto() == province)
			units += e.count;
	}
}

void army::damage(int value) {
	if(value > units)
		value = units;
	units -= value;
	casualty += value;
}