#include "advancement.h"
#include "answers.h"
#include "creature.h"
#include "gender.h"
#include "groupname.h"
#include "ongoing.h"
#include "modifier.h"
#include "monster.h"
#include "pushvalue.h"
#include "roll.h"
#include "script.h"

creature* player;
creature* opponent;

static int dice_table[] = {2, 3, 4, 6, 8, 10, 12, 20};

static ability_s get_attack_damage(ability_s v) {
	switch(v) {
	case MeleeAttack: return MeleeDamage;
	case RangeAttack: return RangeDamage;
	default: return Strenght;
	}
}

dice creature::getdamage(ability_s ability) const {
	auto result = wears[ability].getdamage();
	result.d = maptbl(dice_table, result.d + get(WeaponDiceRaise));
	auto damage_ability = get_attack_damage(ability);
	if(damage_ability)
		result.m += player->get(damage_ability);
	return result;
}

static void wearing(variant v) {
	if(v.iskind<abilityi>())
		player->abilities[v.value] += v.counter;
}

static void wearing(const variants& source) {
	for(auto v : source)
		wearing(v);
}

static void advance(variants source) {
	pushvalue push_modifier(modifier, Permanent);
	for(auto v : source)
		script::run(v);
}

static void update_start() {
	player->load(player->basic);
}

static void update_equipment() {
	for(auto& e : player->equipment()) {
		if(e)
			wearing(e.geti().wearing);
	}
}

static void update_special() {
	player->abilities[AC] += player->getbonus(Dexterity);
	if(player->wears[MeleeWeapon].is(Versatile) && !player->wears[MeleeWeaponOffhand])
		player->abilities[WeaponDiceRaise]++;
}

static void update_ongoing() {
	variant owner = player;
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner)
			wearing(e.effect);
	}
}

static void update_maximum_ability() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		if(player->abilities[i] > 18)
			player->abilities[i] = 18;
	}
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
	enemy = 0xFFFF;
}

void creature::update() {
	pushvalue push(player, this);
	update_start();
	update_equipment();
	update_special();
	update_ongoing();
	update_maximum_ability();
}

bool creature::isallow(const item& v) const {
	auto& ei = v.geti();
	switch(ei.wear) {
	case MeleeWeapon: case RangedWeapon: case Torso:
		return !usable || (usable & FG(getbsi(&ei))) != 0;
	default:
		return true;
	}
}

static void roll_hit_points() {
	auto hits = bsdata<classi>::elements[player->kind].hits;
	auto result = roll(hits, player->is(BonusHitRoll) ? 1 : 0);
	player->basic.abilities[HP] += result;
}

static void roll_random_abilities() {
	while(true) {
		player->basic.rollability();
		auto n = player->basic.getbestability();
		if(player->basic.get(n) >= 14)
			break;
	}
}

static void copy_special_abilities(const statable& e) {
	for(auto i = HP; i <= Dodge; i = (ability_s)(i + 1))
		player->basic.abilities[i] = e.abilities[i];
}

static void transform_ability() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilities[i] = (player->basic.abilities[i] + 5) * 2 + xrand(0, 1);
}

static void starting_hits() {
	player->basic.abilities[HP] += player->basic.getbonus(Constitution);
	if(player->basic.abilities[HP] < 1)
		player->basic.abilities[HP] = 1;
	if(player->is(BonusHitRoll))
		player->basic.abilities[HP] += 2;
}

static void finish_create() {
	player->update();
	for(auto i = HP; i <= Dodge; i = (ability_s)(i + 1))
		player->abilities[i] = player->basic.abilities[i];
}

static bool is_unique_avatar(const char* id) {
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		auto pav = e.getavatar();
		if(pav && equal(pav, id))
			return false;
	}
	return true;
}

const char* random_avatar(class_s type, gender_s gender) {
	auto push_interactive = answers::interactive;
	answers::interactive = false;
	auto result = avatarable::choose(0, gender == Female ? "f*.*" : "m*.*", 6, is_unique_avatar);
	answers::interactive = push_interactive;
	return result;
}

static void random_name() {
	char temp[260]; stringbuilder sb(temp);
	sb.clear(); sb.add("%1%2", bsdata<classi>::elements[player->kind].id, bsdata<genderi>::elements[player->gender].id);
	auto i = groupname::randomid(temp);
	if(i == 0xFFFF) {
		sb.clear(); sb.add("%1%2", bsdata<racei>::elements[player->ancestry].id, bsdata<genderi>::elements[player->gender].id);
		i = groupname::randomid(temp);
	}
	player->name = i;
}

static void apply_advance(variant type) {
	for(auto& e : bsdata<advancement>()) {
		if(e.type == type)
			advance(e.elements);
	}
}

static void apply_level_up() {
	variant type = bsdata<classi>::elements + player->kind;
	type.counter = player->get(Level) + 1;
	apply_advance(type);
	player->abilities[Level]++;
}

void creature::add(race_s race, gender_s gender, class_s kind) {
	player = bsdata<creature>::add();
	player->clear();
	player->ancestry = race;
	player->kind = kind;
	player->gender = gender;
	roll_random_abilities();
	roll_hit_points();
	starting_hits();
	apply_level_up();
	finish_create();
	random_name();
	player->setavatar(random_avatar(player->kind, player->gender));
}

void creature::add(const char* id) {
	add(bsdata<monsteri>::find(id));
}

void creature::add(const monsteri* pm) {
	if(!pm)
		return;
	player = bsdata<creature>::add();
	player->clear();
	player->basic.load(*pm);
	transform_ability();
	copy_special_abilities(*pm);
	advance(pm->feats);
	finish_create();
}

static void remove_enemy(short unsigned id) {
	if(id == 0xFFFF)
		return;
	for(auto& e : bsdata<creature>()) {
		if(e.enemy == id)
			e.enemy = 0xFFFF;
	}
}

void creature::kill() {
	abilities[HP] = 0;
	act(getnm("PlayerDead"));
	remove_enemy(getbsi(this));
}

void creature::damage(int value) {
	act(getnm("SufferDamage"), value);
	auto new_hp = get(HP) - value;
	if(new_hp > 0) {
		abilities[HP] = new_hp;
		return;
	}
	kill();
}

creature* creature::getenemy() const {
	for(auto& e : bsdata<creature>()) {
		if(e.isenemy(*this))
			return &e;
	}
	return 0;
}

const char*	creature::getkindname() const {
	return bsdata<classi>::elements[kind].getname();
}

const char*	creature::getracename() const {
	return bsdata<racei>::elements[ancestry].getname();
}