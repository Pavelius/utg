#include "charname.h"
#include "main.h"

static int ability_bonus[] = {
	-4, -4, -4, -3, -2, -2, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 2, 2, 3
};
static int thac0[3][15] = {
	{20, 19, 19, 19, 17, 17, 17, 14, 14, 14, 12, 12, 12, 10, 10},
	{20, 19, 19, 19, 19, 17, 17, 17, 17, 14, 14, 14, 14, 12, 12},
	{20, 19, 19, 19, 19, 19, 17, 17, 17, 17, 17, 14, 14, 14, 14}
};
static int attack_bonus[3][15] = {
	{0, 0, 0, 0, 2, 2, 2, 5, 5, 5, 7, 7, 7, 9, 9},
	{0, 0, 0, 0, 0, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7},
	{0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 5, 5, 5, 5},
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

int	creature::gethit() const {
	auto n = imin(14, get(Level));
	return attack_bonus[n][bsdata<classi>::elements[type].tohit] + get(ToHit);
}

static unsigned getexperience(unsigned* table, int level) {
	return table[level];
}

void creature::raiselevel() {
	auto n = get(Level);
	auto d = bsdata<classi>::elements[type].hd;
}

void creature::levelup() {
	while(getexperience(fighter_experience, get(Level)) <= experience) {
		add(Level, 1);
		raiselevel();
	}
}

int creature::getbonus(ability_s v) const {
	return maptbl(ability_bonus, get(v));
}

bool creature::attack(ability_s attack, int ac, int bonus) const {
	auto d = 1 + rand() % 20;
	auto r = d + gethit() + bonus;
	switch(attack) {
	case MeleeToHit: r += getbonus(Strenght) + get(MeleeToHit); break;
	case RangedToHit: r += getbonus(Dexterity) + get(RangedToHit); break;
	default: break;
	}
	if(d == 1)
		return false;
	if(d == 20)
		return true;
	return r >= ac;
}

void creature::attack() {
	auto enemy = getenemy();
	auto ac = enemy->get(AC);
	if(attack(MeleeToHit, ac, 0)) {
		actn(getnm("HitMelee"));
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

void creature::create(class_s type, gender_s gender) {
	clear();
	this->type = type;
	this->gender = gender;
	abilities[HP] = 10;
}

const char* creature::randomname(class_s type, gender_s gender) {
	variant collection[] = {
		bsdata<classi>::elements + type,
		bsdata<genderi>::elements + gender
	};
	return charname::getname(charname::random(collection));
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
	act("%герой получил%а [%1i] урона.", value);
	auto hp = get(HP) - value;
	if(hp < -100)
		hp = -100;
	abilities[HP] = hp;
}

bool creature::isready() const {
	return get(HP) > 0;
}