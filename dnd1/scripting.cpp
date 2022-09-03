#include "main.h"
#include "script.h"

creature* player;
creaturea targets;

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
		player->attack();
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
		player->add(ToHit, 2);
		player->attack();
	}
	return true;
}

static chooseoption combat_options[] = {
	{"ChargeEnemy", charge},
	{"AttackMelee", attack_melee},
};

static void combat_round() {
	for(auto p : game.creatures) {
		if(!p->isready())
			continue;
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
	game.rollinitiative();
	while(!draw::isnext() && continue_battle(false)) {
		combat_round();
		chooseoption::choose(end_round_options, 0);
	}
}