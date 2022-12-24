#include "main.h"

void strategyi::getinfo(stringbuilder& sb) const {
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
}

static void addv(stringbuilder& sb, int value, const char* format) {
	sb.addn("[%1i]", value);
	sb.adds(format);
}

static void addv(const playeri* player, stringbuilder& sb, indicator_s value) {
	addv(sb, player->get(value), bsdata<indicatori>::elements[value].getname());
}

static void addp(const playeri* player, stringbuilder& sb, indicator_s value) {
	addv(sb, player->getplanetsummary(value), bsdata<indicatori>::elements[value].getname());
}

static void addv(stringbuilder& sb, const char* value, const char* format) {
	sb.addn(value);
	sb.adds(format);
}

static void addreq(stringbuilder& sb, const char* requiqment) {
	auto pn = sb.get();
	for(auto i = Red; i <= Yellow; i = (color_s)(i + 1)) {
		if(!requiqment[i])
			continue;
		if(pn[0])
			sb.add(",");
		else {
			sb.addn("%Required:");
		}
		sb.adds("%1i %2", requiqment[i], bsdata<colori>::elements[i].getname());
	}
}

static void add_description(stringbuilder& sb, const char* id) {
	auto pn = getdescription(id);
	if(pn) {
		sb.addn("---");
		sb.addn(pn);
	}
}

void techi::getinfo(stringbuilder& sb) const {
	sb.addn("---");
	sb.addn("%1 %-Tech", bsdata<colori>::elements[color].getname());
	addreq(sb, required);
	add_description(sb, id);
}

void playeri::getinfo(stringbuilder& sb) const {
	sb.addn("---");
	addv(this, sb, StrategyToken);
	addv(this, sb, FleetToken);
	addv(this, sb, TacticToken);
	sb.addn("---");
	addp(this, sb, Resources);
	addp(this, sb, Influence);
	add_description(sb, id);
}