#include "player.h"
#include "troop.h"
#include "main.h"

static void add_h3(stringbuilder& sb, const char* format, ...) {
	sb.addn("###");
	sb.addv(format, xva_start(format));
}

void strategyi::getinfo(stringbuilder& sb) const {
	auto pn = getdescription(id);
	if(!pn)
		return;
	add_h3(sb, getname());
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
	add_h3(sb, getname());
	sb.addn("---");
	sb.addn("%1 %-Tech", bsdata<colori>::elements[color].getname());
	addreq(sb, required);
	add_description(sb, id);
}

void playeri::getinfo(stringbuilder& sb) const {
	add_h3(sb, getname());
	sb.addn("---");
	addv(this, sb, StrategyToken);
	addv(this, sb, FleetToken);
	addv(this, sb, TacticToken);
	sb.addn("---");
	addp(this, sb, Resources);
	addp(this, sb, Influence);
	add_description(sb, id);
}

void indicatori::getinfo(stringbuilder& sb) const {
	add_h3(sb, getname());
	add_description(sb, id);
}

static void addv(stringbuilder& sb, const char* format, int value, int count) {
	if(!value)
		return;
	if(count>1)
		sb.addn("%1: %2ix%3i", format, value, count);
	else
		sb.addn("%1: %2i", format, value);
}

static void addcost(stringbuilder& sb, int value, int count) {
	if(!value)
		return;
	if(count > 1)
		sb.addn("%Cost: %1i (%Per %2i %Units)", value, count);
	else
		sb.addn("%Cost: %1i", value);
}

static void addv(stringbuilder& sb, ability_s id, int value, int count = 0) {
	addv(sb, getnm(bsdata<abilityi>::elements[id].id), value, count);
}

void troop::getinfo(stringbuilder& sb) const {
	add_h3(sb, getname());
	sb.addn("---");
	addv(sb, Combat, get(Combat), get(CombatCount));
	addv(sb, AntiFighterBarrage, get(AntiFighterBarrage), get(AntiFighterBarrageCount));
	addv(sb, Bombardment, get(Bombardment), get(BombardmentCount));
	addv(sb, SpaceCannon, get(SpaceCannon), get(SpaceCannonCount));
	addcost(sb, get(Cost), get(CostCount));
	addv(sb, Move, get(Move));
	addv(sb, Production, getproduction());
	addv(sb, Capacity, get(Capacity));
}

static void add_technologies(stringbuilder& sb) {
	for(auto i = (tech_s)0; i < IntegratedEconomy; i = (tech_s)(i + 1)) {
		if(!player->is(i))
			continue;
		sb.addn(bsdata<techi>::elements[i].getname());
	}
}

static void add_action_cards(stringbuilder& sb) {
	for(auto& e : bsdata<entity>()) {
		auto p = e.getactioncard();
		if(!p)
			continue;
		if(e.player != player)
			continue;
		sb.addn(p->getname());
	}
}

template<> void ftstatus<script>(const void* object, stringbuilder& sb) {
	auto p = (script*)object;
	if(equal(p->id, "ShowTech")) {
		add_h3(sb, getnm("Technologies"));
		sb.addn("---");
		add_technologies(sb);
	} else if(equal(p->id, "ShowActionCards")) {
		add_h3(sb, getnm("ActionCards"));
		sb.addn("---");
		add_action_cards(sb);
	}
}