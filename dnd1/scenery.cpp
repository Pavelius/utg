#include "answers.h"
#include "actable.h"
#include "calendar.h"
#include "creature.h"
#include "draw.h"
#include "itemlay.h"
#include "ongoing.h"
#include "pushvalue.h"
#include "randomizer.h"
#include "reaction.h"
#include "roll.h"
#include "scenery.h"
#include "script.h"
#include "speech.h"

scenery* scene;
static scenery*	next_scene;
static spellf scenery_spells;
static int current_milles_distance;
int encountered_count;

void choose_options(variant source);
void combat_mode(int bonus);

static void print(const char* id) {
	printa(scene->geti().id, id);
}

static void scene_adventure() {
	print("Adventure");
}

static void clean_ecnounter() {
	creaturea removes;
	for(auto p : creatures) {
		if(p->is(Player))
			continue;
		removes.add(p);
	}
	for(auto p : removes)
		p->remove();
}

static void play_scene() {
	scene = next_scene; next_scene = 0;
	answers::header = scene->getname();
	answers::resid = scene->geti().getid();
	if(printa(scene->geti().getid(), "Enter"))
		pause();
	while(!draw::isnext()) {
		scene->update();
		scene_adventure();
		script_run(scene->geti().script);
		choose_options(scene->geti().actions);
		pause();
		clean_ecnounter();
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
	scene->type = getbsi(bsdata<scenei>::find(id));
	//scene->flags = scene->geti().flags;
}