#include "answers.h"
#include "calendar.h"
#include "creature.h"
#include "draw.h"
#include "itemlay.h"
#include "actable.h"
#include "scenery.h"
#include "ongoing.h"

scenery* scene;
static scenery* next_scene;
static spellf scenery_spells;

BSDATA(scenefi) = {
	{"Outdoor"},
};
assert_enum(scenefi, Outdoor)

static void play_scene() {
	scene = next_scene; next_scene = 0;
	answers::header = scene->getname();
	answers::resid = scene->geti().getid();
	while(!draw::isnext()) {
		auto id = scene->geti().getid();
		printa(id, "Adventure");
		printa(id, "FindCamp");
		printa(id, "Camp");
		scene->update();
		pause();
	}
}

static void update_spells() {
	scenery_spells.clear();
	variant owner = scene;
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner && e.rounds > current_round)
			scenery_spells.set(e.effect);
	}
}

void scenery::clear() {
	memset(this, 0, sizeof(*this));
	parent = 0xFFFF;
	type = 0xFFFF;
}

bool scenery::is(spell_s v) const {
	return scenery_spells.is(v);
}

void scenery::enter() {
	if(next_scene)
		return;
	next_scene = this;
	draw::setnext(play_scene);
}

bool scenery::haveitems() const {
	return find_container(this) != 0;
}

void scenery::update() {
	update_spells();
	creatures.select();
}

void add_scene(const char* id) {
	scene = bsdata<scenery>::add();
	scene->clear();
	scene->type = getbsi(bsdata<sceneryi>::find(id));
	scene->flags = scene->geti().flags;
}