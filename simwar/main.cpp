#include "answers.h"
#include "army.h"
#include "bsreq.h"
#include "building.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "game.h"
#include "log.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"

using namespace draw;

void next_turn();
void ui_initialize();
void update_provinces();
void util_main();

static void test_army() {
	char temp[1024]; stringbuilder sb(temp);
	army a1;
	a1.clear();
	a1.hero = bsdata<heroi>::find("Gordek");
	a1.player = bsdata<playeri>::find("SouthernKindom");
	// a1.province = province;
	a1.select(province);
	a1.randomtactic();
	sb.add("$image BattleField 0 'art/images'\n");
	a1.act(sb, getnm("ArmyConquest"), province->getname()); sb.addsep(' ');
	a1.act(sb, getdescription(a1.tactic->id));
	answers::message(temp);
}

static void initialize_scene() {
	player = bsdata<playeri>::find("SouthernKindom");
	province = bsdata<provincei>::find("CapeBrumal");
	province->player = player;
	player->upgrade[Lore] += 50;
	script::run(player->start);
	script::run("UpdatePlayer");
	test_army();
	game.initialize();
	update_provinces();
}

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	bsreq::read("maps/silentseas.txt");
	if(log::geterrors())
		return;
	initialize_scene();
	draw::setnext(next_turn);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	ui_initialize();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}