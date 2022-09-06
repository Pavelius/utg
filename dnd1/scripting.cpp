#include "main.h"
#include "script.h"

creature* player;
creaturea targets;
itema items;
spella spells;
static spell_s last_spell;

static void clear_console() {
	if(answers::console)
		answers::console->clear();
}

static void choose_enemies() {
	targets.clear();
	if(player->is(Player)) {
		targets = game.creatures;
		targets.match(Enemy, true);
		targets.matchready(true);
	} else if(player->is(Enemy)) {
		targets = game.creatures;
		targets.match(Player, true);
		targets.matchready(true);
	}
}

static void random_melee_angry(size_t count) {
	choose_enemies();
	targets.matchenemy(false);
	zshuffle(targets.data, targets.count);
	if(count > targets.count)
		count = targets.count;
	for(size_t i = 0; i < count; i++)
		targets.data[i]->setenemy(player);
}

static void choose_player_enemy() {
	if(!player->getenemy()) {
		choose_enemies();
		auto pe = targets.choose(getnm("ChooseTarget"));
		player->setenemy(pe); pe->setenemy(player);
		random_melee_angry(xrand(1, 2));
	}
}

static bool attack_melee(bool run) {
	if(!player->getenemy())
		return false;
	if(run)
		player->meleeattack();
	return true;
}

static bool charge(bool run) {
	if(player->getenemy())
		return false;
	choose_enemies();
	if(!targets)
		return false;
	if(run) {
		choose_player_enemy();
		player->add(MeleeToHit, 1);
		player->add(AC, -1);
		player->meleeattack();
	}
	return true;
}

static bool drink_potion(bool run) {
	items.select(*player);
	items.match(Potion, false);
	if(!items)
		return false;
	if(run) {
	}
	return false;
}

static bool prepare_spells(bool run) {
	spells.known(*player);
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
	spells.prepared(*player);
	if(!spells)
		return false;
	if(run) {
		choose_spell();
		player->cast(spell);
	}
	return true;
}

static chooseoption camp_options[] = {
	{"PrepareSpells", prepare_spells},
};

static chooseoption combat_options[] = {
	{"ChargeEnemy", charge},
	{"AttackMelee", attack_melee},
	{"DrinkPotion", drink_potion},
};

static void combat_round() {
	for(auto p : game.creatures) {
		if(!p->isready())
			continue;
		p->update();
		p->choose(combat_options);
	}
}

static bool lose_game(bool run) {
	targets = game.creatures;
	targets.match(Player, true);
	targets.matchready(true);
	if(targets)
		return false;
	if(run)
		draw::setnext(game.mainmenu);
	return true;
}

static bool win_battle(bool run) {
	targets = game.creatures;
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

static chooseoption end_round_options[] = {
	{"LoseGame", lose_game},
	{"WinBattle", win_battle},
	{"ContinueBattle", continue_battle},
};

void combat_mode() {
	auto push_mode = menu::current_mode;
	menu::current_mode = "Combat";
	game.rollinitiative();
	while(!draw::isnext() && continue_battle(false)) {
		combat_round();
		chooseoption::choose(end_round_options, 0);
	}
	menu::current_mode = push_mode;
}