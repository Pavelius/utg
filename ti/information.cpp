#include "actioncard.h"
#include "player.h"
#include "script.h"
#include "strategy.h"
#include "troop.h"
#include "unit.h"

static void add_h3(stringbuilder& sb, const char* format, ...) {
	sb.addn("###");
	sb.addv(format, xva_start(format));
}

static void add_line(stringbuilder& sb, const char* id, int value) {
	auto nm = iabs(value);
	auto op = value >= 0 ? "Gain" : "Lose";
	auto ct = (nm == 1) ? "Single" : "Many";
	auto pn = getdescription(stw(op, id, ct));
	if(!pn)
		pn = getdescription(stw(op, id, 0));
	if(!pn)
		return;
	sb.addn("* ");
	sb.add(pn, nm);
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

template<> void ftinfo<strategyi>(const void* object, stringbuilder& sb) {
	auto p = (strategyi*)object;
	auto pn = getdescription(p->id);
	if(!pn)
		return;
	add_h3(sb, getnm("PrimaryAbility"));
	for(auto v : p->primary)
		add_line(sb, v.getid(), v.counter);
	add_h3(sb, getnm("SecondaryAbility"));
	for(auto v : p->secondary)
		add_line(sb, v.getid(), v.counter);
}

template<> void ftinfo<techi>(const void* object, stringbuilder& sb) {
	auto p = (techi*)object;
	add_h3(sb, p->getname());
	sb.addn("---");
	sb.addn("%1 %-Tech", bsdata<colori>::elements[p->color].getname());
	addreq(sb, p->required);
	add_description(sb, p->id);
}

template<> void ftinfo<playeri>(const void* object, stringbuilder& sb) {
	auto p = (playeri*)object;
	add_h3(sb, p->getname());
	sb.addn("---");
	addv(p, sb, StrategyToken);
	addv(p, sb, FleetToken);
	addv(p, sb, TacticToken);
	sb.addn("---");
	addp(p, sb, Resources);
	addp(p, sb, Influence);
	auto strategy = p->getstrategy();
	if(strategy) {
		sb.addn("---");
		sb.add(strategy->getname());
	}
	//add_description(sb, p->id);
}

template<> void ftinfo<indicatori>(const void* object, stringbuilder& sb) {
	auto p = (indicatori*)object;
	add_h3(sb, p->getname());
	add_description(sb, p->id);
}

template<> void ftinfo<troop>(const void* object, stringbuilder& sb) {
	auto p = (troop*)object;
	add_h3(sb, p->getname());
	sb.addn("---");
	addv(sb, Combat, p->get(Combat), p->get(CombatCount));
	addv(sb, AntiFighterBarrage, p->get(AntiFighterBarrage), p->get(AntiFighterBarrageCount));
	addv(sb, Bombardment, p->get(Bombardment), p->get(BombardmentCount));
	addv(sb, SpaceCannon, p->get(SpaceCannon), p->get(SpaceCannonCount));
	addcost(sb, p->get(Cost), p->get(CostCount));
	addv(sb, Move, p->get(Move));
	addv(sb, Production, p->getproduction());
	addv(sb, Capacity, p->get(Capacity));
}

template<> void ftinfo<script>(const void* object, stringbuilder& sb) {
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