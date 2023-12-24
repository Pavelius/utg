#include "answers.h"
#include "actable.h"
#include "calendar.h"
#include "creature.h"
#include "draw.h"
#include "itemlay.h"
#include "ongoing.h"
#include "randomizer.h"
#include "roll.h"
#include "scenery.h"
#include "script.h"

scenery* scene;
static scenery*		next_scene;
static spellf		scenery_spells;
static int			current_milles_distance;
static bool			party_surprised, monster_surprised;

void choose_options(variant source);

bool have_feats(feat_s v, bool keep) {
	for(auto p : creatures) {
		if(!p->isready())
			continue;
		if(p->is(v) == keep)
			return true;
	}
	return false;
}

static bool random_monsters_setup() {
	encountered_monster = 0;
	variant v = single(stw(scene->geti().id, "EncounterTable"));
	if(!v)
		return false;
	if(v.iskind<script>())
		bsdata<script>::elements[v.value].proc(0);
	else if(v.iskind<monsteri>())
		encountered_monster = bsdata<monsteri>::elements + v.value;
	if(encountered_monster) {
		auto count = encountered_monster->getcount(WildernessGroup);
	}
	return have_feats(Player, false);
}

static void print(const char* id) {
	printa(scene->geti().id, id);
}

static void scene_adventure() {
	print("Adventure");
}

static void check_random_encounter() {
	if(draw::isnext())
		return;
	// Random encounter roll
	//if(d100() >= scene->geti().encounter_chance)
	//	return;
	if(!random_monsters_setup())
		return;
	party_surprised = false;
	monster_surprised = false;
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
		check_random_encounter();
		//printa(id, "FindCamp");
		//printa(id, "Camp");
		choose_options(scene->geti().actions);
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