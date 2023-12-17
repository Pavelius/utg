#include "advance.h"
#include "answers.h"
#include "creature.h"
#include "modifier.h"
#include "ongoing.h"
#include "pushvalue.h"
#include "roll.h"
#include "script.h"
#include "stringlist.h"

creature *player, *opponent;
creaturea creatures;

static int exp_base_award[] = {
	5,
	10, 20, 35, 75, 175,
	275, 450, 650, 900, 900,
	1100, 1100, 1350, 1350, 1350,
	1350, 2000, 2000, 2000, 2000,
	2500,
};
static int exp_base_award_plus[] = {
	5,
	15, 25, 50, 125, 225,
	350, 450, 650, 900, 900,
	1100, 1100, 1350, 1350, 1350,
	1350, 2000, 2000, 2000, 2000,
	2500,
};
static int exp_bonus_award[] = {
	1,
	3, 5, 15, 50, 125,
	225, 400, 550, 700, 700,
	800, 800, 950, 950, 950,
	950, 1150, 1150, 1150, 1150,
	2000,
};
static int exp_bonus_award_plus[] = {
	1,
	4, 10, 25, 75, 175,
	300, 400, 550, 700, 700,
	800, 800, 950, 950, 950,
	950, 1150, 1150, 1150, 1150,
	2000,
};

static void add_permanent(const variants& source) {
	pushvalue push_modifier(modifier, Permanent);
	script_run(source);
}

static void add_temporary(const variants& source) {
	pushvalue push_modifier(modifier, NoModifier);
	script_run(source);
}

static void apply_advance(variant type, int level) {
	for(auto& e : bsdata<advancei>()) {
		if(e.type == type && e.level == level)
			add_permanent(e.elements);
	}
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

const classi& creature::geti() const {
	return bsdata<classi>::elements[type];
}

int get_feats_ability_count(const creature* player) {
	return 0;
}

int creature::getaward() const {
	auto n = get(Level);
	auto m = getbonus(Constitution);
	auto r = 0;
	if(m > 0)
		r += maptbl(exp_base_award_plus, n);
	else
		r += maptbl(exp_base_award, n);
	auto f = get_feats_ability_count(this);
	if(m > 0)
		r += f * maptbl(exp_bonus_award_plus, n);
	else
		r += f * maptbl(exp_bonus_award, n);
	return r;
}

void creature::raiselevel() {
	auto n = basic.get(Level);
	auto d = bsdata<classi>::elements[type].hd;
	if(d) {
		auto r = rand() % d;
		while(n == 1 && type && r <= 2)
			r = rand() % d;
		basic.abilities[HPMax] += r;
	}
	apply_advance(bsdata<classi>::elements + type, n);
}

static unsigned get_experience(unsigned char type, int level) {
	auto p = bsdata<classi>::elements + type;
	if(level <= 1)
		return 0;
	return maptbl(p->experience, level - 2);
}

void creature::levelup() {
	while(true) {
		auto next_level = basic.get(Level) + 1;
		auto next_experience = get_experience(type, next_level);
		if((next_level > 1 && !next_experience) || (next_experience && experience < next_experience))
			break;
		basic.add(Level, 1);
		raiselevel();
	}
}

//bool creature::attack(ability_s attack, int ac, int bonus) {
//	if(is(Invisibility))
//		dispell(Invisibility);
//	auto d = 1 + (rand() % 20);
//	auto r = d + bonus + abilities[attack];
//	if(d == 1)
//		return false;
//	if(d == 20)
//		return true;
//	return r >= (10 + ac);
//}

void creature::choose(const slice<chooseoption>& options, bool enemy_choose_first) {
	pushvalue push_player(player, this);
	char temp[260]; stringbuilder sb(temp);
	actv(sb, getnm("WhatToDo"), 0, 0);
	const char* enemy_name = 0;
	if(is(Enemy)) {
		if(enemy_choose_first)
			choosef(options);
		else
			chooser(options, temp, enemy_name);
	} else
		::choose(options, temp, enemy_name);
}

const char* random_name(const classi* pi, gender_s gender) {
	return stringlist::getname(stringlist::random(str("%1%2",
		pi->getid(),
		bsdata<genderi>::elements[gender].id)));
}

static bool allow_avatar(const char* avatar) {
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		if(equal(e.avatar, avatar))
			return false;
	}
	return true;
}

const char* random_avatar(const classi* pi, gender_s gender) {
	auto push_interactive = answers::interactive;
	answers::interactive = false;
	auto result = avatarable::choose(0, gender == Female ? "f*.*" : "m*.*", 6, allow_avatar);
	answers::interactive = push_interactive;
	return result;
}

static ongoing* find_bonus(variant owner, spell_s effect) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner && e.effect == effect)
			return &e;
	}
	return 0;
}

void creature::enchant(spell_s effect, unsigned rounds) {
	auto current_round = 0;
	rounds += current_round;
	auto p = find_bonus(this, effect);
	if(!p) {
		p = bsdata<ongoing>::add();
		p->owner = this;
		p->effect = effect;
		p->rounds = rounds;
	} else if(p->rounds < rounds)
		p->rounds = rounds;
}

void creature::dispell(spell_s effect) {
	auto p = find_bonus(this, effect);
	if(p)
		p->clear();
}

void creature::damage(int value) {
	if(value <= 0)
		return;
	if(get(IllusionCopies)) {
		add(IllusionCopies, -1);
		act("Иллюзия %героя рассеялась.", getname());
		return;
	}
	auto hp = get(HP) - value;
	if(hp < -100)
		hp = -100;
	abilities[HP] = hp;
	if(isready())
		act("%герой получил%а [%1i] урона.", value);
	else {
		set(Prone);
		feats.remove(EngageMelee);
		act("%герой получил%а [%1i] урона и упал%а на землю.", value);
	}
}

bool creature::isplayer() const {
	return this == player;
}

bool creature::isready() const {
	return get(HP) > 0;
}

static void update_start() {
	memcpy(player->abilities, player->basic.abilities, sizeof(player->abilities[0]) * (SavePoison + 1));
	player->feats.add(player->basic.feats);
}

static void update_equipment() {
	pushvalue push_modifier(modifier, NoModifier);
	for(auto& e : player->equipment()) {
		if(!e)
			continue;
		add_temporary(e.geti().wearing);
		auto pi = e.getpower();
		if(!pi)
			continue;
		if(pi->wearing)
			add_temporary(pi->wearing);
	}
}

static void update_spells() {
}

void creature::update_finish() {
	// Depended values
	abilities[MeleeToHit] += getbonus(Strenght);
	abilities[MeleeDamage] += getbonus(Strenght);
	abilities[RangedToHit] += getbonus(Dexterity);
	abilities[AC] += getbonus(Dexterity);
	abilities[Speed] += getbonush(Dexterity);
	abilities[HPMax] += getbonus(Constitution) * abilities[Level];
	abilities[SaveDeath] += getbonus(Constitution);
	abilities[SavePoison] += getbonus(Constitution);
	abilities[SaveWands] += getbonush(Dexterity);
	abilities[SaveSpells] += getbonus(Wisdow);
	// Feats depends
	if(is(EngageMelee))
		abilities[ACRanged] += 4;
	if(is(Prone)) {
		abilities[MeleeToHit] -= 2;
		abilities[AC] -= 2;
		abilities[ACRanged] += 4;
	}
	// Maximum hit points
	if(abilities[HPMax] < abilities[Level])
		abilities[HPMax] = abilities[Level];
	// Special spell effect on AC
	if(is(GaseousForm) && abilities[AC] < 11)
		abilities[AC] = 11;
	if(is(Shield) && abilities[AC] < 6)
		abilities[AC] = 6;
}

void creature::update() {
	pushvalue push(player, this);
	update_start();
	update_equipment();
	update_spells();
	update_finish();
}

void finish_creature() {
	player->levelup();
	player->update();
	player->abilities[HP] = player->get(HPMax);
}

dice creature::getdamage(wear_s v) const {
	auto r = wears[v].getdamage();
	switch(v) {
	case MeleeWeapon: r.b += abilities[MeleeDamage]; break;
	case RangedWeapon: r.b += abilities[RangedDamage]; break;
	}
	return r;
}

static void apply_minimal(const classi* pi) {
	for(auto i = 0; i < 6; i++) {
		if(player->basic.abilities[i] < pi->minimal[i])
			player->basic.abilities[i] = pi->minimal[i];
	}
}

static void add_language(const racei* pm) {
	if(!pm)
		return;
	player->languages.set(getbsi(pm));
}

static void add_hits_by_level() {
	dice d = {};
	d.c = player->basic.get(Level);
	d.d = player->geti().hd;
	player->basic.abilities[HPMax] = d.roll();
}

static creature* new_creature() {
	for(auto& e : bsdata<creature>()) {
		if(!e.basic.abilities[Strenght] && e.basic.abilities[HP] <= 0)
			return &e;
	}
	return bsdata<creature>::add();
}

void add_creature(const classi* pi, gender_s gender) {
	player = new_creature();
	player->clear();
	player->type = pi - bsdata<classi>::elements;
	player->gender = gender;
	player->attacks[0].count = 1;
	player->attacks[0].damage = {1, 2};
	player->basic.rollability();
	player->basic.applybest(pi->prime);
	player->name = random_name(pi, gender);
	apply_minimal(pi);
	player->setavatar(random_avatar(pi, gender));
	add_language(pi->origin);
	add_language(bsdata<racei>::elements);
	finish_creature();
}

void add_creature(const monsteri* pi) {
	player = new_creature();
	player->clear();
	player->gender = Male;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilities[i] = 10;
	memcpy(player->attacks, pi->attacks, sizeof(player->attacks));
	add_permanent(pi->feats);
	add_hits_by_level();
	add_language(pi->origin);
	if(player->basic.get(Intellect)>=6 && d100() < 20)
		add_language(bsdata<racei>::elements);
	player->name = getnm(pi->id);
	finish_creature();
}

void creature::drink(spell_s spell) {
	auto& ei = bsdata<spelli>::elements[spell];
	if(ei.isdurable())
		enchant(spell, (6 + d6()) * 10);
	else
		apply(spell, 10, true);
}

void creature::use(item& it) {
}

bool creature::isallow(const item& it) const {
	static variant last_item = "Shield";
	auto index = getbsi(&it.geti());
	if(index >= last_item.value)
		return true;
	return (geti().allow & (1 << index)) != 0;
}

void creature::equip(item& v) {
	if(!isallow(v))
		return;
	wearable::equip(v);
}