#include "main.h"

gamei		game;
int			last_choose, last_page, last_scene;
ability_s	last_ability;
actioni*	last_action;

static void generate_classes() {
	auto push_interactive = utg::interactive;
	utg::interactive = false;
	for(auto& e : bsdata<pirate>())
		e.generate();
	utg::interactive = push_interactive;
}

void gamei::clear() {
	memset(this, 0, sizeof(*this));
}

void gamei::generate() {
	game.pirate::clear();
	game.pirate::randomname();
	game.pirate::generate();
}

void gamei::choosehistory() {
	game.pirate::choosehistory();
	utg::sb.clear();
}

static void fixerror(const char* id, ...) {
	utg::sb.addn("[-");
	utg::sb.addv(getnm(id), xva_start(id));
	utg::sb.addn("]");
}

static void special_command(special_s v, int bonus) {
	switch(v) {
	case Roll:
		game.roll();
		break;
	case Choose:
		last_choose = bonus;
		break;
	case Page000: last_page = bonus; break;
	case Page100: last_page = 100 + bonus; break;
	case Page200: last_page = 200 + bonus; break;
	case Page300: last_page = 300 + bonus; break;
	case Page400: last_page = 400 + bonus; break;
	case Page500: last_page = 500 + bonus; break;
	case Page600: last_page = 600 + bonus; break;
	case Page700: last_page = 700 + bonus; break;
	case Page800: last_page = 800 + bonus; break;
	case Page900: last_page = 900 + bonus; break;
	case Scene:
		if(!bonus)
			last_scene = game.scene;
		else
			last_scene = bonus;
		break;
	default:
		break;
	}
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability:
		last_ability = (ability_s)v.value;
		if(v.counter)
			game.set(last_ability, game.get(last_ability) + v.counter);
		break;
	case Special:
		special_command((special_s)v.value, v.counter);
		break;
	}
}

void gamei::apply(const variants& source) {
	last_ability = Infamy;
	for(auto v : source)
		apply(v);
}

locationi& gamei::getlocation() {
	return bsdata<locationi>::elements[scene];
}

void gamei::createtreasure() {
	auto m = bsdata<treasurei>::source.getcount();
	for(unsigned i = 0; i < m; i++)
		treasures.add(i);
	zshuffle(treasures.data, treasures.count);
}

const treasurei* gamei::picktreasure() {
	if(!treasures.count)
		return 0;
	auto pv = bsdata<treasurei>::elements + treasures.data[0];
	treasures.remove(0);
	return pv;
}

void gamei::sfgetproperty(const void* object, variant v, stringbuilder& sb) {
	switch(v.type) {
	case Ability:
		pirate::sfgetproperty(static_cast<pirate*>(&game), v, sb);
		break;
	}
}

void gamei::playscene() {
	game.getlocation().play();
}