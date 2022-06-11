#include "main.h"

army*				army::last;
static army			attacker, defender;
int					army::round;
static int			hits;
static const char*	new_block;

static bool isother(const entitya& source, const entity* object, tag_s v) {
	for(auto p : source) {
		if(p == object)
			continue;
		if(p->is(v))
			return true;
	}
	return false;
}

static int unit_combat_roll(int chance, int count, int bonus, int reroll) {
	if(!chance)
		return 0;
	if(!count)
		count = 1;
	auto total = 0;
	for(auto i = 0; i < count; i++) {
		auto r = reroll + 1;
		while(r-- > 0) {
			auto result = 1 + (rand() % 10) + bonus;
			if(answers::console) {
				if(answers::console->get() != new_block)
					answers::console->add(", ");
			}
			if(result >= chance) {
				total++;
				if(answers::console)
					answers::console->add("[%1i]", result);
				break;
			} else {
				if(answers::console)
					answers::console->add("%1i", result);
			}
		}
	}
	return total;
}

int army::roll(ability_s id, ability_s id_count) const {
	auto push_last = playeri::last;
	playeri::last = player;
	auto result = 0;
	auto bonus = 0;
	if(player->is(Unrelenting))
		bonus++;
	for(auto p : units) {
		auto chance = p->get(id);
		if(!chance)
			continue;
		if(answers::console) {
			answers::console->addn("%1: ", p->getname());
			new_block = answers::console->get();
		}
		auto count = p->get(id_count);
		auto unit_bonus = bonus;
		if(isother(units, p, CombatBonusToOthers))
			unit_bonus++;
		result += unit_combat_roll(chance, count, unit_bonus, 0);
	}
	playeri::last = push_last;
	return result;
}

static int find_sustain(entitya& source) {
	auto m = source.getcount();
	for(auto i = 0; i < m; i++) {
		auto p = source[i];
		if(p->is(SustainDamage) && !p->is(Exhaust))
			return source.indexof(p);
	}
	return -1;
}

void army::hit(int value) {
	while(value--) {
		if(!units)
			break;
		auto index = find_sustain(units);
		if(index==-1)
			index = units.count - 1;
		auto p = units.data[index];
		p->hit();
		if(!(*p))
			units.remove(index, 1);
	}
}

bool start_combat(const entity* location) {
	attacker.clear();
	defender.clear();
	if(!location)
		return false;
	defender.player = location->player;
	if(!defender.player)
		return false;
	attacker.player = location->getenemy();
	if(!attacker.player)
		return false;
	defender.units.select(defender.player, location);
	defender.units.sortunit();
	if(!defender.units)
		return false;
	attacker.units.select(attacker.player, location);
	attacker.units.sortunit();
	if(!attacker.units)
		return false;
	return true;
}

void combat_continue(int bonus, int param) {
	choosestep::stop = true;
}

void combat_reatreat(int bonus, int param) {
	army::last->reatreat = true;
	choosestep::stop = true;
}

void army::choose(const char* id) {
	auto push_last = last;
	last = this;
	auto push_player = playeri::last;
	playeri::last = player;
	auto push_human = choosestep::human;
	choosestep::human = playeri::last->ishuman();
	choosestep::run(id);
	choosestep::human = push_human;
	playeri::last = push_player;
	last = push_last;
}

void entity::startcombat() {
	if(!start_combat(this))
		return;
	draw::pause();
	auto push_header = answers::header;
	answers::header = getnm(bsdata<abilityi>::elements[AntiFighterBarrage].id);
	defender.hit(attacker.roll(AntiFighterBarrage, AntiFighterBarrageCount));
	attacker.hit(attacker.roll(AntiFighterBarrage, AntiFighterBarrageCount));
	draw::pause();
	do {
		army::round++;
		answers::header = getnm("SpaceCombat");
		attacker.choose("ChooseCombatOption");
		defender.choose("ChooseCombatOption");
		auto hits_attacker = attacker.roll(Combat, CombatCount);
		if(answers::console)
			answers::console->addn("---");
		auto hits_defender = defender.roll(Combat, CombatCount);
		if(answers::console)
			answers::console->addn("---");
		if(hits_attacker || hits_defender) {
			defender.hit(hits_attacker);
			attacker.hit(hits_defender);
		} else
			draw::output(getnm("NoCasualties"));
		game.updateui();
	} while(!attacker.reatreat && !defender.reatreat && attacker.units && defender.units);
	draw::pause();
	answers::header = push_header;
}