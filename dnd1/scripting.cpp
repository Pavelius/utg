#include "collection.h"
#include "creature.h"
#include "draw_utg.h"
#include "game.h"
#include "rolldice.h"
#include "script.h"
#include "spell.h"

creaturea targets;
itema items;
spella spells;
static spell_s last_spell;
static int critical_roll;

template<> void fnscript<rolldice>(int index, int bonus) {
	bonus += last_roll;
	last_roll_raw = bsdata<rolldice>::elements[index].value.roll();
	last_roll = last_roll_raw + bonus;
	output("[{%1i%+2i=%3i}]", last_roll_raw, bonus, last_roll);
}

static bool rolld20(int bonus, int dc) {
	bonus += last_roll;
	last_roll_raw = xrand(1, 20);
	last_roll = last_roll_raw + bonus;
	critical_roll = 0;
	output("[{%1i%+2i=%3i}]", last_roll_raw, bonus, last_roll);
	if(last_roll_raw == 20)
		critical_roll = 1;
	else if(last_roll_raw == 1)
		critical_roll = -1;
	return last_roll >= dc || (critical_roll == 1);
}

static bool make_attack(ability_s attack, int ac, int bonus) {
	if(player->is(Invisibility))
		player->dispell(Invisibility);
	return rolld20(bonus, 10 + ac);
}

static void melee_attack(int bonus) {
	auto ac = opponent->get(AC);
	if(make_attack(MeleeToHit, ac, 0)) {
		player->actn(getnm("HitMelee"));
		auto& weapon = player->wears[MeleeWeapon];
		auto result = weapon.getdamage().roll();
		result += player->get(MeleeDamage);
		opponent->damage(xrand(1, 6));
	} else
		player->actn(getnm("MissMelee"));
}

static void clear_console() {
	if(answers::console)
		answers::console->clear();
}

static void update_enemies() {
	for(auto p : creatures) {
		if(p->enemy && !p->enemy->isready())
			p->enemy = 0;
	}
}

void select_enemies(int bonus) {
	targets.clear();
	if(player->is(Player)) {
		targets = creatures;
		targets.match(Enemy, true);
		targets.matchready(true);
	} else if(player->is(Enemy)) {
		targets = creatures;
		targets.match(Player, true);
		targets.matchready(true);
	}
}
static bool allow_targets(int bonus) {
	last_script->proc(bonus);
	return targets.getcount() != 0;
}

static int compare_initiative(const void* v1, const void* v2) {
	auto p1 = *((creature**)v1);
	auto p2 = *((creature**)v2);
	return p1->initiative - p2->initiative;
}

void roll_initiative() {
	for(auto p : creatures)
		p->initiative = xrand(1, 6);
	qsort(creatures.data, creatures.count, sizeof(creatures.data[0]), compare_initiative);
}

creature* choose_target() {
	answers an;
	for(auto p : targets)
		an.add(p, p->getname());
	return (creature*)an.choose(getnm("ChooseTarget"));
}

static void create_monsters(const monsteri* pm, int count, feat_s feat) {
	for(auto i = 0; i < count; i++) {
		auto p = bsdata<creature>::add();
		p->create(*pm);
		if(feat)
			p->set(feat);
	}
	if(count > 1)
		output(getnm("AppearSeveral"), pm->getname());
	else
		output(getnm("AppearSingle"), pm->getname());
}

static void random_encounter(const monsteri* pm) {
	if(!pm)
		return;
	create_monsters(pm, pm->dungeon.roll(), Enemy);
}

void random_encounter(const char* id) {
	random_encounter(bsdata<monsteri>::find(id));
}

static void random_melee_angry(size_t count) {
	select_enemies(0);
	targets.matchenemy(false);
	zshuffle(targets.data, targets.count);
	if(count > targets.count)
		count = targets.count;
	for(size_t i = 0; i < count; i++)
		targets.data[i]->setenemy(player);
}

static void choose_player_enemy() {
	if(!player->enemy) {
		select_enemies(0);
		auto pe = targets.choose(getnm("ChooseTarget"), player->is(Enemy));
		player->setenemy(pe); pe->setenemy(player);
		random_melee_angry(xrand(1, 2));
	}
}

static bool attack_melee(bool run) {
	if(!player->enemy)
		return false;
	if(run) {
		player->meleeattack();
		update_enemies();
	}
	return true;
}

static bool attack_range(bool run) {
	if(!player->enemy)
		return false;
	if(run) {
		player->meleeattack();
		update_enemies();
	}
	return true;
}

static bool charge(bool run) {
	if(player->enemy)
		return false;
	select_enemies(0);
	if(!targets)
		return false;
	if(run) {
		choose_player_enemy();
		player->add(MeleeToHit, 1);
		player->add(AC, -1);
		player->meleeattack();
		update_enemies();
	}
	return true;
}

static bool drink_potion(bool run) {
	items.select(player->allitems());
	items.match(Potion, false);
	if(!items)
		return false;
	if(run) {
	}
	return false;
}

static bool prepare_spells(bool run) {
	spells.select(player->known_spells);
	if(!spells)
		return false;
	if(run) {

	}
	return true;
}

static void choose_spell() {
	answers an;
	for(auto spell : spells)
		an.add((void*)spell, bsdata<spelli>::elements[spell].getname());
	last_spell = (spell_s)(int)an.choose(getnm("ChooseSpellToCast"));
}

static bool cast_spells(bool run) {
	spells.select(*player);
	if(!spells)
		return false;
	if(run) {
		choose_spell();
		player->cast(last_spell);
	}
	return true;
}

static chooseoption camp_options[] = {
	{"PrepareSpells", prepare_spells},
};

static void combat_round() {
	static chooseoption combat_options[] = {
		{"ChargeEnemy", charge},
		{"AttackMelee", attack_melee},
		{"AttackRange", attack_range},
		{"DrinkPotion", drink_potion},
	};
	for(auto p : creatures) {
		if(!p->isready())
			continue;
		p->update();
		p->choose(combat_options);
	}
}

static bool lose_game(bool run) {
	targets = creatures;
	targets.match(Player, true);
	targets.matchready(true);
	if(targets)
		return false;
	if(run)
		draw::setnext(game.mainmenu);
	return true;
}

static bool win_battle(bool run) {
	targets = creatures;
	targets.match(Enemy, true);
	targets.matchready(true);
	if(targets)
		return false;
	if(run)
		draw::setnext(game.mainmenu);
	return true;
}

static bool continue_battle(bool run) {
	if(win_battle(false) || lose_game(false))
		return false;
	if(run)
		clear_console();
	return true;
}

void combat_mode() {
	static chooseoption options[] = {
		{"LoseGame", lose_game},
		{"WinBattle", win_battle},
		{"ContinueBattle", continue_battle},
	};
	auto push_mode = menu::current_mode;
	menu::current_mode = "Combat";
	roll_initiative();
	while(!draw::isnext() && continue_battle(false)) {
		combat_round();
		choose(options, 0);
	}
	menu::current_mode = push_mode;
}

static void all_saves(int bonus) {
	player->add(SaveDeath, bonus);
	player->add(SaveWands, bonus);
	player->add(SaveParalize, bonus);
	player->add(SaveBreathWeapon, bonus);
	player->add(SaveSpells, bonus);
}

BSDATA(script) = {
	{"Saves", all_saves},
};
BSDATAF(script)