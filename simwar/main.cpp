#include "answers.h"
#include "building.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"
#include "unit.h"

using namespace draw;

void ui_initialize();
void update_provinces();
void player_turn();

static answers an;

void player_turn();
extern fnevent input_province;

static void end_turn() {
}

static void choose_province() {
	buttonparam();
}

static void choose_units() {
	an.clear();
	for(auto& e : bsdata<troop>()) {
		if(e.province != province)
			continue;
		an.add(&e, e.type->getname());
	}
	auto push_header = answers::header;
	answers::header = province->getname();
	auto pu = an.choose(0, getnm("Cancel"));
	answers::header = push_header;
	if(!pu)
		return;
}

static void choose_buildings() {
}

static void province_options() {
	an.add(choose_units, getnm("Units"));
	an.add(choose_buildings, getnm("Buildings"));
}

static void* choose_answers() {
	pushvalue push_answers(answers::header);
	pushvalue push_input(input_province, choose_province);
	if(province)
		answers::header = province->getname();
	an.clear();
	if(province) {
		province_options();
		return an.choose(0, getnm("Cancel"));
	} else {
		an.add(end_turn, getnm("EndTurn"));
		return an.choose();
	}
}

static void accept_answers(void* result) {
	if(!result)
		province = 0;
	else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else
		((fnevent)result)();
	draw::setnext(player_turn);
}

static void player_turn() {
	auto p = choose_answers();
	accept_answers(p);
}

static void initialize_scene() {
	player = bsdata<playeri>::find("SouthernKindom");
	province = bsdata<provincei>::find("SnakeForest");
	player->upgrade[Lore] += 50;
	script::run(player->start);
	script::run("UpdatePlayer");
	update_provinces();
}

static void start_game() {
	initialize_scene();
	draw::setnext(player_turn);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	ui_initialize();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}