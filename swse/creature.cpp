#include "advance.h"
#include "creature.h"
#include "script.h"

creature *player;
collection<creature> creatures;

static void advance_creature(int level, variant object) {
	for(auto& e : bsdata<advancei>()) {
		if(e.level == level && e.object == object)
			runscript(e.elements);
	}
}

static void clear_creature(char* pd, const char* ps) {
	memcpy(pd, ps, Armor + 1);
}

void creature::update_ability() {
	auto level = gethlevel();
	abilities[Reflex] += 10 + getbonus(Dexterity) + imax(level / 2, get(Armor));
	abilities[Fortitude] += 10 + getbonus(Constitution) + level / 2;
	abilities[Will] += 10 + getbonus(Wisdow) + level / 2;
}

void creature::update() {
	clear_creature(abilities, basic.abilities);
	update_ability();
}

int	creature::gethlevel() const {
	auto r = 0;
	for(auto i = (class_s)Jedi; i <= Soldier; i = (class_s)(i + 1))
		r += classes[i];
	return r;
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::add(class_s v) {
	auto push_player = player; player = this;
	advance_creature(++classes[v], bsdata<classi>::elements + v);
	player = push_player;
}

void creature::create(class_s type, gender_s gender) {
	clear();
	setgender(gender);
	add(type);
}