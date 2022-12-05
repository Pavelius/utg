#include "advance.h"
#include "creature.h"
#include "feat.h"
#include "script.h"

creature *player;
collection<creature> creatures;

static void advance_creature(int level, variant object) {
	for(auto& e : bsdata<advancei>()) {
		if(e.level == level && e.object == object)
			runscript(e.elements);
	}
}

static void clear_creature(char* pd, const char* ps) {
	memcpy(pd, ps, Armor + 1);
}

static bool test_prerequisit(const creature* player, variant v) {
	if(v.iskind<abilityi>())
		return player->get((ability_s)v.value) >= v.value;
	else if(v.iskind<feati>())
		return player->feats.is(v.value);
	else if(v.iskind<itemi>())
		return player->isitem(v.value, v.counter);
	return true;
}

static bool test_prerequisit(const creature* player, const variants& source) {
	for(auto v : source) {
		if(!test_prerequisit(player, v))
			return false;
	}
	return true;
}

void creature::update_ability() {
	auto level = gethlevel();
	auto armor_dexterity_bonus = getbonus(Dexterity);
	if(armor_dexterity_bonus > get(MaxDexterityBonus))
		armor_dexterity_bonus = get(MaxDexterityBonus);
	auto armor = isweararmor() ? get(Armor) : level;
	abilities[Reflex] += 10 + get(Armor);
	if(!is(Flatfooted))
		abilities[Reflex] += armor_dexterity_bonus + get(DodgeBonus);
	abilities[Fortitude] += 10 + getbonus(Constitution) + level + get(EquipmentBonus);
	abilities[Will] += 10 + getbonus(Wisdow) + level;
}

void creature::update() {
	clear_creature(abilities, basic.abilities);
	update_ability();
}

int	creature::getlevel() const {
	auto r = 0;
	for(auto v : classes)
		r += v;
	return r;
}

int	creature::gethlevel() const {
	auto r = 0;
	for(auto i = (class_s)Jedi; i <= Soldier; i = (class_s)(i + 1))
		r += classes[i];
	return r;
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::add(class_s v) {
	auto push_player = player; player = this;
	advance_creature(++classes[v], bsdata<classi>::elements + v);
	player = push_player;
}

void creature::create(class_s type, gender_s gender) {
	clear();
	setgender(gender);
	add(type);
}