#include "main.h"

static bool			stop_combat, win_fight;
static cardprotoi	enemy;

static void add_combat_items(bool need_equiped) {
	auto hands = player::last->getfreehands();
	for(auto& e : cards) {
		if(e.area != PlayerArea)
			continue;
		auto& ei = e.geti();
		if(!need_equiped && (!ei.hands || ei.hands > hands))
			continue;
		auto equiped = player::last->isequiped(&e);
		if(need_equiped != equiped)
			continue;
		auto format = equiped ? "UnequipItem" : "EquipItem";
		an.add(&e, getnm(format), getnm(e.geti().id));
	}
}

int player::getcombat() const {
	auto combat_bonus = get(Fight) + get(Combat);
	for(auto p : hands) {
		if(p)
			combat_bonus += p->geti().getcombat(enemy);
	}
	combat_bonus += enemy.get(Combat);
	return combat_bonus;
}

int player::getevade() const {
	return get(Sneak) + get(Escape) + enemy.get(Escape);
}

int player::gethorror() const {
	return get(Will) + get(Horror) + enemy.get(Horror);
}

int	player::getfreehands() const {
	auto used = 0;
	for(auto p : hands) {
		if(p)
			used += p->geti().hands;
	}
	return 2 - used;
}

bool player::evade(cardprotoi& enemy) {
	return roll(Sneak, getevade());
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
	auto bonus = player::last->getcombat();
	player::last->usehands();
	if(player::last->roll(Fight, bonus) >= enemy.get(Toughness)) {
		stop_combat = true;
		win_fight = true;
	} else
		player::last->damage(Health, enemy.get(CombatDamage));
	test_ready();
}

static void horror_check() {
	if(!enemy.get(HorrorDamage))
		return;
	auto bonus = player::last->gethorror();
	if(player::last->roll(Will, bonus) <= 0)
		player::last->damage(Sanity, enemy.get(HorrorDamage));
}

static fnevent combat_options(bool before_start);

static void start_fight() {
	horror_check();
	while(!stop_combat) {
		auto proc = combat_options(false);
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

static fnevent combat_options(bool before_start) {
	char temp[260]; stringbuilder sb(temp);
	while(true) {
		sb.clear(); player::last->equipment(sb);
		an.clear();
		if(before_start) {
			an.add(start_fight, getnm("StartFight"));
			an.add(start_evade, getnm("TryEvade"), player::last->getevade());
		} else {
			an.add(try_fight, getnm("TryFight"), player::last->getcombat(), enemy.get(Toughness));
			if(!enemy.is(Ambush))
				an.add(try_evade, getnm("TryEvade"), player::last->getevade());
		}
		add_combat_items(true);
		add_combat_items(false);
		auto result = an.choose(temp, 0);
		if(!result)
			break;
		if(cards.indexof(result) != -1) {
			auto p = (cardi*)result;
			if(player::last->isequiped(p))
				player::last->unequip(p);
			else
				player::last->equip(p);
		} else
			return (fnevent)result;
	}
	return 0;
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
	auto proc = combat_options(true);
	if(proc)
		proc();
	answers::prompt = push_prompt;
	answers::header = push_header;
	last = push_last;
	return win_fight;
}