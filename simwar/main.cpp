#include "answers.h"
#include "building.h"
#include "calendar.h"
#include "collection.h"
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
	calendar.turn++;
}

static void choose_province() {
	buttonparam();
}

static bool player_troop(const void* pv) {
	return ((troop*)pv)->player == player;
}

static bool player_province_troop(const void* pv) {
	return ((troop*)pv)->player == player && ((troop*)pv)->province == province;
}

static bool player_building(const void* pv) {
	return ((building*)pv)->player == player;
}

static bool player_province_building(const void* pv) {
	return ((building*)pv)->player == player && ((building*)pv)->province == province;
}

static void select_units(collection<troop>& source) {
}

static void choose_units() {
	pushvalue push_image(answers::resid, "Units");
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
	pushvalue push_image(answers::resid, "Buildings");
	an.clear();
	collection<building> buildings; buildings.select(player_province_building);
	for(auto p : buildings)
		an.add(p, p->type->getname());
	auto push_header = answers::header;
	answers::header = province->getname();
	auto pu = an.choose(0, getnm("Cancel"));
	answers::header = push_header;
	if(!pu)
		return;
}

static void add_answers(fnevent proc, const char* id, int count) {
	if(count > 0)
		an.add(proc, "%1 (%2i %Piece)", getnm(id), count);
	else
		an.add(proc, getnm(id));
}

static void province_options() {
	collection<troop> troops; troops.select(player_province_troop);
	if(troops)
		add_answers(choose_units, "Units", troops.getcount());
	collection<building> buildings; buildings.select(player_province_building);
	add_answers(choose_buildings, "Buildings", buildings.getcount());
}

static void* choose_answers() {
	pushvalue push_answers(answers::header);
	pushvalue push_input(input_province, choose_province);
	pushvalue push_image(answers::resid);
	if(province) {
		answers::header = province->getname();
		answers::resid = province->landscape->id;
	}
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
	else if(bsdata<troop>::have(result)) {
	} else if(bsdata<building>::have(result)) {
	} else
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
	calendar.year = 1410;
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