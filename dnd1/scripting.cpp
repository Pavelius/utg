#include "main.h"
#include "script.h"

static bool test_attack_melee() {
	auto player = creature::last;
	game.targets.clear();
	if(player) {
		if(player->is(Player)) {
			game.targets = game.creatures;
			game.targets.match(Enemy, true);
		} else if(player->is(Enemy)) {
			game.targets = game.creatures;
			game.targets.match(Player, true);
		}
	}
	return game.targets.count != 0;
}

static bool test_lose_game() {
	game.targets.match(Player, true);
	return game.targets.count == 0;
}

static void attack_melee() {
}

static void lose_game() {
	draw::setnext(game.mainmenu);
}

static bool test_win_battle() {
	game.targets.match(Enemy, true);
	return game.targets.count == 0;
}

static void win_battle() {
	draw::setnext(game.mainmenu);
}

BSDATA(chooseoption) = {
	{"AttackMelee", attack_melee, test_attack_melee},
	{"LoseGame", lose_game, test_lose_game},
	{"WinBattle", win_battle, test_win_battle},
};
BSDATAF(chooseoption)