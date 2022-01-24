#include "main.h"

gamei		game;
int			last_count;
int			last_choose, last_page;
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
	}
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability:
		last_ability = (ability_s)v.value;
		last_count = v.counter;
		if(last_count)
			game.set(last_ability, game.get(last_ability) + last_count);
		break;
	case Special:
		special_command((special_s)v.value, v.counter);
		break;
	}
}

void gamei::apply(const variants& source) {
	last_ability = Infamy;
	last_bonus = 0;
	last_choose = 0;
	last_page = 0;
	for(auto v : source)
		apply(v);
}

locationi& gamei::getlocation() {
	return bsdata<locationi>::elements[location];
}

void gamei::play() {
	auto push_header = utg::header;
	auto push_image = utg::url;
	auto& loc = game.getlocation();
	utg::header = getnm(loc.id);
	if(loc.image)
		utg::url = loc.image;
	utg::sb.clear();
	game.chooseactions();
	game.playround();
	utg::header = push_header;
	utg::url = push_image;
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

void gamei::getpropertyst(const void* object, variant v, stringbuilder& sb) {
	switch(v.type) {
	case Ability:
		pirate::getpropertyst(static_cast<pirate*>(&game), v, sb);
		break;
	}
}