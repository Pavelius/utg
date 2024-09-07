#include "advance.h"
#include "creature.h"
#include "feat.h"
#include "pushvalue.h"
#include "rand.h"
#include "script.h"

creature *player, *opponent;
collection<creature> creatures, opponents;

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
	enemy_id = 0xFFFF;
	setnoname();
}

void creature::add(class_s v) {
	pushvalue push_player(player, this);
	advance_creature(++classes[v], bsdata<classi>::elements + v);
}

relation_s creature::getrelation() const {
	auto r = get(Relation);
	if(r < -50)
		return Hostile;
	else if(r < -20)
		return Unfriendly;
	else if(r < 20)
		return Neutrality;
	else if(r < 50)
		return Friendly;
	else
		return Helpful;
}

static relation_s getmorale(relation_s v) {
	switch(v) {
	case Helpful: case Friendly: case Neutrality: return Friendly;
	default: return Hostile;
	}
}

void creature::setenemy(const creature* p) {
	enemy_id = getbsi(p);
}

bool creature::isenemy(const creature* p) const {
	return getmorale(getrelation()) != getmorale(p->getrelation());
}

bool creature::ismeleefight() const {
	for(auto p : creatures) {
		if(p->getenemy() == this && p->getrange(this) == 0)
			return true;
	}
	return false;
}

creature* creature::getenemy() const {
	if(enemy_id != 0xFFFF)
		return bsdata<creature>::elements + enemy_id;
	return 0;
}

static int compare_int(const void* v1, const void* v2) {
	return *((int*)v2) - *((int*)v1);
}

static int roll_dices(int c, int d, int b, int k) {
	static int result[16];
	if(c > (int)(sizeof(result) / sizeof(result[0])))
		c = sizeof(result) / sizeof(result[0]);
	for(auto i = 0; i < c; i++)
		result[i] = rand() % d;
	qsort(result, c, sizeof(result[0]), compare_int);
	if(k) {
		if(c > k)
			c = k;
	}
	for(auto i = 0; i < c; i++)
		b += result[i];
	return b;
}

static int roll4d6m() {
	return roll_dices(4, 6, 0, 3);
}

static void random_ability() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilities[i] = roll4d6m();
}

void create_hero(class_s type, gender_s gender) {
	player = bsdata<creature>::add();
	player->clear();
	player->setgender(gender);
	random_ability();
	player->add(type);
	player->update();
}

void add_creatures() {
	creatures.add(player);
}

void add_item(const char* id) {
	auto pi = bsdata<itemi>::find(id);
	if(!pi)
		return;
	last_item = 0;
	item it; it.create(pi);
	player->equip(it);
}