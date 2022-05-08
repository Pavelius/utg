#include "main.h"

static bool			stop_combat, win_fight;
static cardprotoi	enemy;

bool player::evade(cardprotoi& enemy) {
	return roll(Sneak, enemy.get(Escape));
}

static void test_ready() {
	if(!player::last->isready())
		stop_combat = true;
}

static void try_evade() {
	if(player::last->evade(enemy))
		stop_combat = true;
	else
		player::last->damage(Health, enemy.get(CombatDamage));
	test_ready();
}

static void try_fight() {
	if(player::last->roll(Fight, enemy.get(Combat)) >= enemy.get(Toughness)) {
		stop_combat = true;
		win_fight = true;
	} else
		player::last->damage(Health, enemy.get(CombatDamage));
	test_ready();
}

static fnevent combat_choose() {
	an.clear();
	an.add(try_fight, getnm("TryFight"), enemy.get(Combat), enemy.get(Toughness));
	an.add(try_evade, getnm("TryEvade"), enemy.get(Escape));
	return (fnevent)an.choose(0, 0);
}

static void horror_check() {
	if(player::last->roll(Will, enemy.get(Horror)) <= 0)
		player::last->damage(Sanity, enemy.get(HorrorDamage));
}

static void start_fight() {
	horror_check();
	while(!stop_combat) {
		auto proc = combat_choose();
		if(!proc)
			break;
		proc();
		test_ready();
	}
}

static void start_evade() {
	try_evade();
	if(stop_combat)
		return;
	start_fight();
}

static fnevent start_combat_choose() {
	an.add(start_fight, getnm("StartFight"));
	an.add(start_evade, getnm("TryEvade"), enemy.get(Escape));
	return (fnevent)an.choose(0, 0);
}

bool player::fight(cardi& source) {
	enemy = source.geti();
	// TODO: prepare monster stat for stat depending
	stop_combat = false;
	win_fight = false;
	auto push_prompt = answers::prompt;
	auto push_header = answers::header;
	auto push_last = last; last = this;
	answers::header = getnm(enemy.id);
	auto p = getdescription(enemy.id);
	if(p)
		answers::prompt = p;
	auto proc = start_combat_choose();
	if(proc)
		proc();
	answers::prompt = push_prompt;
	answers::header = push_header;
	last = push_last;
	return win_fight;
}