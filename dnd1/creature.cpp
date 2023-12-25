#include "advance.h"
#include "answers.h"
#include "creature.h"
#include "gender.h"
#include "modifier.h"
#include "ongoing.h"
#include "pushvalue.h"
#include "roll.h"
#include "scenery.h"
#include "script.h"
#include "speech.h"

creature *player, *opponent;

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
	leader = 0xFFFF;
	name = -1;
}

creature* creature::getleader() const {
	return getbs<creature>(leader);
}

void creature::setleader(const creature* pv) {
	leader = getbsi(pv);
}

const classi& creature::geti() const {
	if(type.iskind<classi>())
		return bsdata<classi>::elements[type.value];
	return bsdata<classi>::elements[0];
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

bool creature::ischaracter() const {
	return type.iskind<classi>();
}

void creature::raiselevel() {
	auto pc = &geti();
	auto n = basic.get(Level);
	auto d = pc->hd;
	if(d) {
		auto r = 1 + rand() % d;
		if(ischaracter() && n==1) {
			while(r <= 2)
				r = rand() % d;
		}
		r += basic.getbonus(Constitution);
		if(r < 1)
			r = 1;
		basic.abilities[HPMax] += r;
	}
	apply_advance(type, n);
}

void creature::remove() {
	clear();
	creatures.remove(this);
}

static unsigned get_experience(const classi& ei, int level) {
	if(level <= 1)
		return 0;
	return maptbl(ei.experience, level - 2);
}

void creature::levelup() {
	while(true) {
		auto next_level = basic.get(Level) + 1;
		auto next_experience = get_experience(geti(), next_level);
		if((next_level > 1 && !next_experience) || (next_experience && experience < next_experience))
			break;
		basic.add(Level, 1);
		raiselevel();
	}
}

int random_name(const classi* pi, gender_s gender) {
	return speech_random(str("%1%2", pi->getid(), bsdata<genderi>::elements[gender].id));
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

bool creature::isenemy() const {
	return player->is(Player) ? is(Enemy) : is(Player);
}

bool creature::isally() const {
	return player->is(Player) ? is(Player) : is(Enemy);
}

bool creature::isready() const {
	return get(HP) > 0;
}

bool creature::iswounded() const {
	return get(HP) < get(HPMax);
}

static void update_start() {
	memcpy(player->abilities, player->basic.abilities, sizeof(player->abilities[0]) * (SpellLevel6 + 1));
	player->feats.add(player->basic.feats);
}

static void update_equipment() {
	pushvalue push_modifier(modifier, NoModifier);
	for(auto& e : player->equipment()) {
		if(!e)
			continue;
		add_temporary(e.geti().wearing);
	}
}

static void update_spells() {
	variant owner = player;
	player->active_spells.clear();
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner)
			player->active_spells.set(e.effect);
	}
}

static void random_prepare_spells() {
	player->prepared.clear();
	for(auto level = 1; level <= 6; level++) {
		auto number = player->getspells(level);
		if(!number)
			continue;
		spella known_spells;
		known_spells.select(player->known_spells);
		known_spells.match(player->geti().magic, level, true);
		if(!known_spells)
			continue;
		known_spells.shuffle();
		auto index = 0;
		while(number-- > 0) {
			auto pi = known_spells[index];
			player->prepared.spells[getbsi(pi)]++;
			index++;
			if(index >= (int)known_spells.count)
				index = 0;
		}
	}
}

bool creature::save(spell_s spell, int& count) const {
	auto& ei = bsdata<spelli>::elements[spell];
	auto id = ei.getsave();
	if(!id)
		return false; // No save variant;
	auto result = rolld20(get(id), 20, false);
	if(result) {
		// If save success alway show message
		if(!actid(ei.id, "SaveSuccess", ' '))
			actid("Common", "SaveSuccess", ' ');
	} else
		actid(ei.id, "SaveFailed", ' ');
	return result;
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
	if(is(SunSensitive) && scene->is(SunSensitive))
		abilities[MeleeToHit] -= 2;
	// Maximum hit points
	if(abilities[HPMax] < abilities[Level])
		abilities[HPMax] = abilities[Level];
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
	random_prepare_spells();
	player->rest();
}

interval creature::getdamage(wear_s v) const {
	auto r = wears[v].getdamage();
	switch(v) {
	case MeleeWeapon:
		r.min += abilities[MeleeDamage];
		r.max += abilities[MeleeDamage];
		break;
	case RangedWeapon:
		r.min += abilities[RangedDamage];
		r.max += abilities[RangedDamage];
		break;
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
	interval d = {};
	d.min = player->basic.get(Level);
	d.max = d.min * player->geti().hd;
	player->basic.abilities[HPMax] = d.roll();
}

static creature* new_creature() {
	for(auto& e : bsdata<creature>()) {
		if(!e.basic.abilities[Strenght] && e.basic.abilities[HP] <= 0)
			return &e;
	}
	return bsdata<creature>::add();
}

void add_creature(const classi* pi, gender_s gender, int level) {
	player = new_creature();
	player->clear();
	player->type = pi;
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
	player->experience = get_experience(player->geti(), level);
	finish_creature();
}

void add_creature(const monsteri* pi) {
	player = new_creature();
	player->clear();
	player->type = pi;
	player->gender = Male;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilities[i] = 10;
	memcpy(player->attacks, pi->attacks, sizeof(player->attacks));
	add_permanent(pi->feats);
	add_hits_by_level();
	add_language(pi->origin);
	if(player->basic.get(Intellect)>=6 && d100() < 20)
		add_language(bsdata<racei>::elements);
	finish_creature();
}

void creature::drink(spell_s spell) {
	auto& ei = bsdata<spelli>::elements[spell];
	if(ei.isdurable())
		enchant(this, this, spell, (6 + d6()) * 10);
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

void creature::rest() {
	*static_cast<spellable*>(this) = prepared;
}

int	creature::getspells(int level) const {
	auto n = (ability_s)(SpellLevel1 + level - 1);
	return get(n);
}

void creature::kill() {
	abilities[HP] = 0;
}

int	creature::getlevel(spell_s spell) const {
	return bsdata<spelli>::elements[spell].level[geti().magic];
}

int get_average(ability_s v) {
	auto total = 0;
	auto count = 0;
	for(auto p : creatures) {
		if(!p->isready() || !p->is(Player))
			continue;
		total += p->get(v);
		count++;
	}
	if(!count)
		return 0;
	return total / count;
}

int get_minimal(ability_s v) {
	auto total = 0;
	for(auto p : creatures) {
		if(!p->isready() || !p->is(Player))
			continue;
		auto n = p->get(v);
		if(total < n)
			continue;
		total = n;
	}
	return total;
}

int get_maximal(ability_s v) {
	auto total = 0;
	for(auto p : creatures) {
		if(!p->isready() || !p->is(Player))
			continue;
		auto n = p->get(v);
		if(total > n)
			continue;
		total = n;
	}
	return total;
}