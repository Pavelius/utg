#include "main.h"

struct army {
	playeri*		player;
	entitya			units;
	bool			reatreat;
	static army*	last;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			option();
};
army*				army::last;
static army			attacker, defender;
static int			hits;

static int combat_roll(entitya& source, ability_s id, ability_s id_count) {
	auto result = 0;
	for(auto p : source) {
		auto chance = p->get(id);
		if(!chance)
			continue;
		auto count = p->get(id_count);
		result += p->fight(chance, count);
	}
	return result;
}

static int attackers_roll(ability_s id, ability_s id_count) {
	auto push_last = playeri::last;
	playeri::last = attacker.player;
	auto result = combat_roll(attacker.units, id, id_count);
	playeri::last = push_last;
	return result;
}

static int defenders_roll(ability_s id, ability_s id_count) {
	auto push_last = playeri::last;
	playeri::last = defender.player;
	auto result = combat_roll(defender.units, id, id_count);
	playeri::last = push_last;
	return result;
}

static void assign_hits(playeri* player, entitya& source, int hits) {
	while(hits--) {
		if(!source)
			break;
		auto p = source.data[--source.count];
		if(player->ishuman())
			draw::warning("%1 %-Destoyed", p->getname());
		else
			draw::information("%1 %-Destoyed", p->getname());
		p->clear();
	}
}

static void hit_defender(int hits) {
	assign_hits(defender.player, defender.units, hits);
}

static void hit_attacker(int hits) {
	assign_hits(attacker.player, attacker.units, hits);
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

static void add_choose(const char* id) {
	choosestep::run("ChooseCombatOption");
}

void army::option() {
	auto push_last = last;
	last = this;
	auto push_player = playeri::last;
	playeri::last = player;
	auto push_human = choosestep::human;
	choosestep::human = playeri::last->ishuman();
	add_choose("ChooseCombatOption");
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
	hit_defender(attackers_roll(AntiFighterBarrage, AntiFighterBarrageCount));
	hit_attacker(defenders_roll(AntiFighterBarrage, AntiFighterBarrageCount));
	draw::pause();
	do {
		answers::header = getnm("SpaceCombat");
		attacker.option();
		defender.option();
		auto hits_attacker = attackers_roll(Combat, CombatCount);
		auto hits_defender = defenders_roll(Combat, CombatCount);
		hit_defender(hits_attacker);
		hit_attacker(hits_defender);
	} while(!attacker.reatreat && !defender.reatreat && attacker.units && defender.units);
	draw::pause();
	answers::header = push_header;
}