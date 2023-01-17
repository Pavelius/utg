#include "charname.h"
#include "creature.h"
#include "ongoing.h"

creature* player;
creaturea creatures;

inline int d6() { return 1 + rand() % 6; }

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

void creature::rangeattack(creature* enemy) {
	auto ac = enemy->get(AC);
	auto& weapon = wears[RangedWeapon];
	if(attack(RangedToHit, ac, 0)) {
		actn(getnm("HitRange"));
		auto result = weapon.getdamage().roll();
		result += get(RangedDamage);
		enemy->damage(result);
	} else
		actn(getnm("MissRange"));
}

void creature::meleeattack() {
	auto ac = enemy->get(AC);
	auto& weapon = wears[MeleeWeapon];
	if(attack(MeleeToHit, ac, 0)) {
		actn(getnm("HitMelee"));
		auto result = weapon.getdamage().roll();
		result += get(MeleeDamage);
		enemy->damage(xrand(1, 6));
	} else
		actn(getnm("MissMelee"));
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
	const char* enemy_name = enemy ? enemy->getname() : 0;
	if(is(Enemy))
		chooseoption::chooser(options, temp, enemy_name);
	else
		chooseoption::choose(options, temp, enemy_name);
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

void creature::setenemy(creature* v) {
	enemy = v;
}

void creature::damage(int value) {
	if(value <= 0)
		return;
	if(get(IllusionCopies)) {
		add(IllusionCopies, -1);
		act("Иллюзия %$1 рассеялась.", getname());
		return;
	}
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

void creature::update_start() {
	memcpy(abilities, basic.abilities, sizeof(abilities[0]) * (SavePoison + 1));
}

void creature::update_equipment() {
	//for(auto i = MeleeWeapon; i <= Elbows; i = (wear_s)(i + 1))
	//	equipmentbonus(wears[i]);
}

void creature::update_spells() {
}

void creature::update_finish() {
	// Basic values
	abilities[ToHit] += getattackbonus(bsdata<classi>::elements[type].tohit, abilities[Level]);
	// Depended values
	abilities[MeleeToHit] += abilities[ToHit] + getbonus(Strenght);
	abilities[MeleeDamage] += abilities[Damage] + getbonus(Strenght);
	abilities[RangedToHit] += abilities[ToHit] + getbonus(Dexterity);
	abilities[RangedDamage] += abilities[Damage];
	abilities[AC] += getbonus(Dexterity);
	abilities[Speed] += getbonush(Dexterity);
	abilities[HPMax] += getbonus(Constitution) * abilities[Level];
	// Saves
	if(is(BestowCurse))
		abilities[Saves] -= 2;
	abilities[SaveDeath] += abilities[Saves] + getbonus(Constitution);
	abilities[SavePoison] += abilities[Saves] + getbonus(Constitution);
	abilities[SaveWands] += abilities[Saves] + getbonush(Dexterity);
	abilities[SaveParalize] += abilities[Saves];
	abilities[SaveBreathWeapon] += abilities[Saves];
	abilities[SaveSpells] += abilities[Saves] + getbonus(Wisdow);
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

void creature::create(const struct monsteri& v) {
	clear();
	this->type = Monster;
	this->gender = Male;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		basic.abilities[i] = 10;
	this->feats = v.feats;
	name = getnm(v.id);
	finish();
}

void creature::drink(spell_s spell) {
	auto& ei = bsdata<spelli>::elements[spell];
	if(ei.isdurable())
		enchant(spell, (6 + d6()) * 10);
	else
		apply(spell, 10, true);
}

void creature::use(item& it) {
	auto& ei = it.geti();
	auto pe = it.getenchant();
	switch(ei.wear) {
	case Potion:
		if(pe) {
			if(pe->special.iskind<spelli>())
				drink((spell_s)pe->special.value);
		}
		break;
	}
}