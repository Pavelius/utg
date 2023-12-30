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
static spellf	scenery_spells;
static int		current_milles_distance;
int				encountered_count;

void choose_options(variant source);
void combat_mode(int bonus);
bool have_feats(feat_s side, feat_s v, bool keep);
void reaction_roll(int bonus);

static int group_size(int count) {
	if(count <= 1)
		return 0;
	else if(count <= 3)
		return 1;
	else if(count <= 7)
		return 2;
	else
		return 3;
}

static const char* phrase(const char* id, int param) {
	auto p = speech_find(id);
	if(!p)
		return 0;
	return speech_get(p, param);
}

static void set_feat(bool party, feat_s v) {
	for(auto p : creatures) {
		if(p->is(Player) != party)
			continue;
		p->set(v);
	}
}

static void add_monster(const monsteri* pc) {
	pushvalue push(player);
	add_creature(pc);
	if(player) {
		creatures.add(player);
		encountered.add(player);
	}
}

static void add_monsters() {
	encountered_count = encountered_monster->getcount(WildernessGroup).roll();
	for(auto i = 0; i < encountered_count; i++)
		add_monster(encountered_monster);
	script_run(encountered_monster->wilderness_group);
}

static bool random_monsters_setup() {
	encountered_monster = 0;
	encountered.clear();
	variant v = single(stw(scene->geti().id, "EncounterTable"));
	if(!v)
		return false;
	if(v.iskind<script>())
		bsdata<script>::elements[v.value].proc(0);
	else if(v.iskind<monsteri>())
		encountered_monster = bsdata<monsteri>::elements + v.value;
	if(encountered_monster)
		add_monsters();
	return have_feats(Player, false);
}

static void print(const char* id) {
	printa(scene->geti().id, id);
}

static void scene_adventure() {
	print("Adventure");
}

static const char* get_monster_pluar() {
	if(!encountered_monster)
		return "";
	static char temp[260]; stringbuilder sb(temp);
	auto pe = getnme(stw(encountered_monster->id, "Pluar"));
	if(pe)
		return pe;
	pe = getnm(encountered_monster->id);
	sb.addpl(getnm(encountered_monster->id));
	return temp;
}

static void look_monsters() {
	printn(speech_get("LookGroup"), phrase("GroupSize", group_size(encountered_count)));
	prints(speech_get("LookGroupDetail"), get_monster_pluar());
}

static void make_ambush() {
	printn(speech_get("AnyMonsterWildernessAmbush"));
	set_feat(false, Enemy);
	combat_mode(0);
}

static void random_reaction() {
	reaction_roll(0);
}

static void check_random_encounter() {
	if(draw::isnext())
		return;
	// Random encounter roll
	//if(d100() >= scene->geti().encounter_chance)
	//	return;
	if(!random_monsters_setup())
		return;
	//random_surprise();
	random_reaction();
	//if(party_surprised && !monster_surprised)
	//	make_ambush();
	//else if(reaction <= Unfriendly) {
	//	set_feat(false, Enemy);
	//	combat_mode(0);
	//} else
	//	look_monsters();
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
		check_random_encounter();
		//printa(id, "FindCamp");
		//printa(id, "Camp");
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