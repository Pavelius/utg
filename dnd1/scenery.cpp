#include "calendar.h"
#include "draw.h"
#include "itemlay.h"
#include "scenery.h"
#include "ongoing.h"

scenery* scene;
static scenery* next_scene;
static spellf scenery_spells;

static void play_scene() {
	scene = next_scene; next_scene = 0;
}

static void update_spells() {
	scenery_spells.clear();
	variant owner = scene;
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner && e.rounds > current_round)
			scenery_spells.set(e.effect);
	}
}

void scenery::enter() {
	if(next_scene)
		return;
	next_scene = this;
	draw::setnext(play_scene);
}

bool scenery::haveitems() const {
	variant v = this;
	for(auto& e : bsdata<itemlay>()) {
		if(e.parent == v)
			return true;
	}
	return false;
}

void scenery::update() {
	update_spells();
}