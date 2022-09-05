#include "charname.h"
#include "main.h"

static int ability_bonus[] = {
	-4, -4, -4, -3, -2, -2, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 2, 2, 3
};
static int ability_bonush[] = {
	-3, -3, -3, -2, -1, -1, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 2
};
static int attack_bonus[3][15] = {
	{-1, 0, 0, 0, 2, 2, 2, 5, 5, 5, 7, 7, 7, 9, 9},
	{-1, 0, 0, 0, 0, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7},
	{-1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 5, 5, 5, 5},
};
static unsigned fighter_experience[] = {
	0, 0, 2000, 4000, 8000, 16000, 32000, 64000, 120000, 240000,
	360000, 480000, 600000, 720000, 840000,
};
static unsigned elf_experience[] = {
	0, 0, 4000, 8000, 16000, 32000, 64000, 120000, 250000, 400000,
	480000, 600000,
};
static unsigned dwarf_experience[] = {
	0, 0, 2200, 4400, 8800, 17000, 35000, 70000, 140000, 270000,
	400000, 530000, 660000,
};

void creature::clear() {
	memset(this, 0, sizeof(*this));
	enemy_index = 0xFF;
}

static void start_equipment(creature* p) {
	item it;
	for(auto& e : bsdata<equipmenti>()) {
		if(e.type == p->type) {
			it.create(bsdata<itemi>::elements + e.equipment);
			p->equip(it);
			if(it)
				p->additem(it);
		}
	}
}

static unsigned getexperience(unsigned* table, int level) {
	return table[level];
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
}

void creature::levelup() {
	while(getexperience(fighter_experience, basic.get(Level) + 1) <= experience) {
		basic.add(Level, 1);
		raiselevel();
	}
}

int creature::getbonus(ability_s v) const {
	return maptbl(ability_bonus, get(v));
}

int creature::getbonush(ability_s v) const {
	return maptbl(ability_bonush, get(v));
}

bool creature::attack(ability_s attack, int ac, int bonus) const {
	auto d = 1 + (rand() % 20);
	auto r = d + bonus + abilities[ToHit] + abilities[attack];
	if(d == 1)
		return false;
	if(d == 20)
		return true;
	return r >= (10 + ac);
}

void creature::rangeattack(creature* enemy) {
	auto ac = enemy->get(AC);
	auto& weapon = wears[RangedWeapon];
	if(attack(RangedToHit, ac, 0)) {
		actn(getnm("HitRange"));
		auto result = weapon.getdamage().roll();
		result += get(Damage) + get(RangedDamage);
		enemy->damage(result);
	} else
		actn(getnm("MissRange"));
}

void creature::meleeattack() {
	auto enemy = getenemy();
	auto ac = enemy->get(AC);
	auto& weapon = wears[MeleeWeapon];
	if(attack(MeleeToHit, ac, 0)) {
		actn(getnm("HitMelee"));
		auto result = weapon.getdamage().roll();
		result += get(Damage) + get(MeleeDamage);
		enemy->damage(xrand(1, 6));
	} else
		actn(getnm("MissMelee"));
}

bool creature::isactive(spell_s v) const {
	return ongoing::find(v, this) != 0;
}

feat_s creature::getenemyfeat() const {
	if(is(Player))
		return Enemy;
	else if(is(Enemy))
		return Player;
	return Undead;
}

void creature::choose(const slice<chooseoption>& options) {
	auto push_last = player;
	player = this;
	char temp[260]; stringbuilder sb(temp);
	actv(sb, getnm("WhatToDo"), 0, 0);
	chooseoption::choose(options, temp);
	player = push_last;
}

const char* creature::randomname(class_s type, gender_s gender) {
	variant collection[] = {
		bsdata<classi>::elements + type,
		bsdata<genderi>::elements + gender
	};
	return charname::getname(charname::random(collection));
}

const char* creature::randomavatar(class_s type, gender_s gender) {
	auto push_interactive = answers::interactive;
	answers::interactive = false;
	auto result = avatarable::choose(0, gender==Female ? "f*.*" : "m*.*");
	answers::interactive = push_interactive;
	return result;
}

static bonusi* find_bonus(variant owner, spell_s effect) {
	for(auto& e : bsdata<bonusi>()) {
		if(e.owner == owner && e.effect == effect)
			return &e;
	}
	return 0;
}

void creature::enchant(spell_s effect, unsigned rounds) {
	rounds += game.rounds;
	auto p = find_bonus(this, effect);
	if(!p) {
		p = bsdata<bonusi>::add();
		p->owner = this;
		p->effect = effect;
		p->rounds = game.rounds + rounds;
	} else if(p->rounds < rounds)
		p->rounds = rounds;
}

void creature::dispell(spell_s effect) {
	auto p = find_bonus(this, effect);
	if(p)
		p->clear();
}

void creature::setenemy(const creature* v) {
	if(!v)
		enemy_index = 0xFF;
	else
		enemy_index = v - bsdata<creature>::elements;
}

void creature::damage(int value) {
	if(value <= 0)
		return;
	auto hp = get(HP) - value;
	if(hp < -100)
		hp = -100;
	abilities[HP] = hp;
	if(isready())
		act("%герой получил%а [%1i] урона.", value);
	else
		act("%герой получил%а [%1i] урона и упал%а на землю.", value);
}

bool creature::isready() const {
	return get(HP) > 0;
}

static void copyvalues(statable& v1, statable& v2) {
	v1 = v2;
}

void creature::update_equipment() {
	for(auto i = MeleeWeapon; i <= Elbows; i = (wear_s)(i + 1))
		equipmentbonus(wears[i]);
}

void creature::update() {
	auto hp = abilities[HP];
	copyvalues(*this, basic);
	abilities[HP] = hp;
	update_equipment();
	auto level = abilities[Level];
	abilities[ToHit] += maptbl(attack_bonus[bsdata<classi>::elements[type].tohit], level);
	abilities[MeleeToHit] += getbonus(Strenght);
	abilities[RangedToHit] += getbonus(Dexterity);
	abilities[MeleeDamage] += getbonus(Strenght);
	abilities[AC] += getbonus(Dexterity);
	abilities[Speed] += getbonush(Dexterity);
	abilities[HPMax] += getbonus(Constitution);
	abilities[SaveWands] += getbonush(Dexterity);
	abilities[SaveDeath] += getbonus(Constitution);
	abilities[SavePoison] += getbonus(Constitution);
	abilities[SaveSpells] += getbonus(Wisdow);
	if(abilities[HPMax] < abilities[Level])
		abilities[HPMax] = abilities[Level];
}

void creature::finish() {
	levelup();
	update();
	abilities[HP] = get(HPMax);
}

dice creature::getdamage(wear_s v) const {
	auto r = wears[v].getdamage();
	r.b += abilities[Damage];
	switch(v) {
	case MeleeWeapon: r.b += abilities[MeleeDamage]; break;
	case RangedWeapon: r.b += abilities[RangedDamage]; break;
	}
	return r;
}

void creature::create(class_s type, gender_s gender) {
	clear();
	this->type = type;
	this->gender = gender;
	basic.rollability();
	basic.applybest(bsdata<classi>::elements[type].prime);
	basic.applyminimal(type);
	name = randomname(type, gender);
	setavatar(randomavatar(type, gender));
	start_equipment(this);
	finish();
}

void creature::use(item& it) {
	auto& ei = it.geti();
	switch(ei.wear) {
	case Potion:
		break;
	}
}