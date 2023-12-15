#include "collection.h"
#include "creature.h"
#include "draw_utg.h"
#include "game.h"
#include "modifier.h"
#include "pushvalue.h"
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
}

template<> void fnscript<abilityi>(int index, int value) {
	switch(modifier) {
	case Permanent: player->basic.abilities[index] += value; break;
	default: player->abilities[index] += value; break;
	}
}

template<> void fnscript<itemi>(int index, int value) {
	auto count = value ? value : 1;
	item it; it.create(index, count);
	player->equip(it);
	if(it)
		player->additem(it);
}

template<> void fnscript<feati>(int index, int value) {
	switch(modifier) {
	case Permanent: player->basic.feats.set(index, value >= 0); break;
	default: player->feats.set(index, value >= 0); break;
	}
}

static bool rolld20(int bonus, int dc) {
	last_roll_raw = xrand(1, 20);
	last_roll = last_roll_raw + bonus;
	critical_roll = 0;
	if(last_roll_raw == 20)
		critical_roll = 1;
	else if(last_roll_raw == 1)
		critical_roll = -1;
	return last_roll >= dc || (critical_roll == 1);
}

static void make_attack(const char* id, int bonus, ability_s attack, ability_s damage, wear_s weapon) {
	if(player->is(Invisibility))
		player->dispell(Invisibility);
	auto ac = opponent->get(AC);
	if(rolld20(bonus, 10 + ac)) {
		information("{%1i%+2i=%3i}", last_roll_raw, bonus, last_roll);
		player->actid("Hit", id, ' ');
		auto& hands = player->wears[weapon];
		auto result = hands.getdamage().roll();
		result += player->get(damage);
		opponent->damage(result);
	} else {
		warning("{%1i%+2i=%3i}", last_roll_raw, bonus, last_roll);
		player->actid("Miss", id, ' ');
	}
}

static void melee_attack(int bonus) {
	make_attack("Melee", bonus, MeleeToHit, MeleeDamage, MeleeWeapon);
}

static void range_attack(int bonus) {
	make_attack("Ranged", bonus, RangedToHit, RangedDamage, RangedWeapon);
}

static bool is_fight_melee(int bonus) {
	return player->is(EngageMelee) == (bonus >= 0);
}

static void clear_console() {
	if(answers::console)
		answers::console->clear();
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

static void add_monsters(const monsteri* pm, int count, feat_s feat) {
	pushvalue push_player(player);
	for(auto i = 0; i < count; i++) {
		add_creature(*pm);
		if(feat)
			player->set(feat);
	}
	if(count > 1)
		output(getnm("AppearSeveral"), pm->getname());
	else
		output(getnm("AppearSingle"), pm->getname());
}

static void random_encounter(const monsteri* pm) {
	if(!pm)
		return;
	add_monsters(pm, pm->dungeon.roll(), Enemy);
}

void random_encounter(const char* id) {
	random_encounter(bsdata<monsteri>::find(id));
}

static void random_melee_angry(size_t count) {
	select_enemies(0);
	targets.match(EngageMelee, false);
	zshuffle(targets.data, targets.count);
	if(count > targets.count)
		count = targets.count;
	for(size_t i = 0; i < count; i++)
		targets.data[i]->set(EngageMelee);
}

static void choose_player_enemy() {
	select_enemies(0);
	opponent = targets.choose(getnm("ChooseTarget"), player->is(Enemy));
	if(opponent) {
		player->set(EngageMelee);
		opponent->set(EngageMelee);
		if(rand() % 2)
			random_melee_angry(1);
	}
}

static bool attack_melee(bool run) {
	select_enemies(0);
	if(!targets)
		return false;
	if(run) {
		choose_player_enemy();
		melee_attack(0);
	}
	return true;
}

static bool attack_range(bool run) {
	select_enemies(0);
	if(!targets)
		return false;
	if(run) {
		opponent = targets.choose(getnm("ChooseTarget"), player->is(Enemy));
		range_attack(0);
	}
	return true;
}

static bool charge(bool run) {
	if(player->is(EngageMelee))
		return false;
	select_enemies(0);
	if(!targets)
		return false;
	if(run) {
		choose_player_enemy();
		player->add(MeleeToHit, 1);
		player->add(AC, -1);
		melee_attack(0);
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
		//{"AttackRange", attack_range},
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
	{"FightingMelee", script_none, is_fight_melee},
	{"Saves", all_saves},
};
BSDATAF(script)