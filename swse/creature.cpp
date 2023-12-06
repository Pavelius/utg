#include "advance.h"
#include "creature.h"
#include "feat.h"
#include "pushvalue.h"
#include "script.h"

creature *player;
collection<creature> creatures;

static void advance_creature(int level, variant object) {
	for(auto& e : bsdata<advancei>()) {
		if(e.level == level && e.object == object)
			script_run(e.elements);
	}
}

static void clear_creature(char* pd, const char* ps) {
	memcpy(pd, ps, Armor + 1);
}

static bool test_prerequisit(variant v) {
	if(v.iskind<abilityi>())
		return player->get((ability_s)v.value) >= v.value;
	else if(v.iskind<feati>())
		return player->feats.is(v.value);
	else if(v.iskind<itemi>())
		return player->isitem(v.value, v.counter);
	return true;
}

static bool test_prerequisit(const variants& source) {
	for(auto v : source) {
		if(!test_prerequisit(v))
			return false;
	}
	return true;
}

static void update_ability() {
	auto level = player->gethlevel();
	auto armor_dexterity_bonus = player->getbonus(Dexterity);
	if(armor_dexterity_bonus > player->get(MaxDexterityBonus))
		armor_dexterity_bonus = player->get(MaxDexterityBonus);
	auto armor = player->isweararmor() ? player->get(Armor) : level;
	player->abilities[Reflex] += 10 + player->get(Armor);
	if(!player->is(Flatfooted))
		player->abilities[Reflex] += armor_dexterity_bonus + player->get(DodgeBonus);
	player->abilities[Fortitude] += 10 + player->getbonus(Constitution) + level + player->get(EquipmentBonus);
	player->abilities[Will] += 10 + player->getbonus(Wisdow) + level;
}

void creature::update() {
	pushvalue push_player(player, this);
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
	pushvalue push_player(player, this);
	advance_creature(++classes[v], bsdata<classi>::elements + v);
}

void creature::create(class_s type, gender_s gender) {
	clear();
	setgender(gender);
	add(type);
}