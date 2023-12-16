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
		if(e.type == type && e.level==level)
			add_permanent(e.elements);
	}
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
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

void creature::levelup() {
	while(getexperience(type, basic.get(Level) + 1) <= experience) {
		basic.add(Level, 1);
		raiselevel();
	}
}

bool creature::attack(ability_s attack, int ac, int bonus) {
	if(is(Invisibility))
		dispell(Invisibility);
	auto d = 1 + (rand() % 20);
	auto r = d + bonus + abilities[attack];
	if(d == 1)
		return false;
	if(d == 20)
		return true;
	return r >= (10 + ac);
}

feat_s creature::getenemyfeat() const {
	if(is(Player))
		return Enemy;
	else if(is(Enemy))
		return Player;
	return Undead;
}

void creature::choose(const slice<chooseoption>& options) {
	pushvalue push_player(player, this);
	char temp[260]; stringbuilder sb(temp);
	actv(sb, getnm("WhatToDo"), 0, 0);
	const char* enemy_name = 0;
	if(is(Enemy))
		chooser(options, temp, enemy_name);
	else
		::choose(options, temp, enemy_name);
}

const char* creature::randomname(class_s type, gender_s gender) {
	return stringlist::getname(stringlist::random(str("%1%2",
		bsdata<classi>::elements[type].getid(),
		bsdata<genderi>::elements[gender].id)));
}

const char* creature::randomavatar(class_s type, gender_s gender) {
	auto push_interactive = answers::interactive;
	answers::interactive = false;
	auto result = avatarable::choose(0, gender==Female ? "f*.*" : "m*.*", 6);
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
		act("������� %����� ����������.", getname());
		return;
	}
	auto hp = get(HP) - value;
	if(hp < -100)
		hp = -100;
	abilities[HP] = hp;
	if(isready())
		act("%����� �������%� [%1i] �����.", value);
	else {
		set(Prone);
		feats.remove(EngageMelee);
		act("%����� �������%� [%1i] ����� � ����%� �� �����.", value);
	}
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
	// Basic values
	auto thac0 = getattackbonus(bsdata<classi>::elements[type].tohit, abilities[Level]);
	// Depended values
	abilities[MeleeToHit] += thac0 + getbonus(Strenght);
	abilities[MeleeDamage] += getbonus(Strenght);
	abilities[RangedToHit] += thac0 + getbonus(Dexterity);
	abilities[AC] += getbonus(Dexterity);
	abilities[Speed] += getbonush(Dexterity);
	abilities[HPMax] += getbonus(Constitution) * abilities[Level];
	abilities[SaveDeath] += getbonus(Constitution);
	abilities[SavePoison] += getbonus(Constitution);
	abilities[SaveWands] += getbonush(Dexterity);
	abilities[SaveSpells] += getbonus(Wisdow);
	if(is(Prone)) {
		abilities[MeleeToHit] -= 2;
		abilities[AC] -= 2;
	}
	// Finale saves transformation
	for(auto i = SaveDeath; i <= SavePoison; i = (ability_s)(i + 1)) {
		abilities[i] = getsave(type, i, abilities[Level]) - abilities[i];
		if(abilities[i] < 2)
			abilities[i] = 2;
		else if(abilities[i] > 20)
			abilities[i] = 20;
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

void creature::finish() {
	levelup();
	update();
	abilities[HP] = get(HPMax);
}

dice creature::getdamage(wear_s v) const {
	auto r = wears[v].getdamage();
	switch(v) {
	case MeleeWeapon: r.b += abilities[MeleeDamage]; break;
	case RangedWeapon: r.b += abilities[RangedDamage]; break;
	}
	return r;
}

void creature::create(class_s type, gender_s gender) {
	pushvalue push_player(player, this);
	auto pi = bsdata<classi>::elements + type;
	clear();
	this->type = type;
	this->gender = gender;
	attacks[0].count = 1;
	attacks[0].damage = {1, 2};
	basic.rollability();
	basic.applybest(pi->prime);
	basic.applyminimal(type);
	name = randomname(type, gender);
	setavatar(randomavatar(type, gender));
	apply_advance(pi, 1);
	finish();
}

void add_creature(const struct monsteri& v) {
	player = bsdata<creature>::add();
	player->clear();
	player->type = Monster;
	player->gender = Male;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilities[i] = 10;
	memcpy(player->attacks, v.attacks, sizeof(player->attacks));
	add_permanent(v.feats);
	player->name = getnm(v.id);
	player->finish();
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