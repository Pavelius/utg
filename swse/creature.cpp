#include "advance.h"
#include "creature.h"
#include "feat.h"
#include "math.h"
#include "modifier.h"
#include "pushvalue.h"
#include "rand.h"
#include "script.h"
#include "stringvar.h"

creature *player, *opponent;
collection<creature> creatures, opponents;

static void advance_creature(int level, variant object) {
	pushvalue push(modifier, Permanent);
	for(auto& e : bsdata<advancei>()) {
		if(e.object.counter == level && e.object.type == object.type && e.object.value == object.value)
			script_run(e.elements);
	}
}

static void clear_creature() {
	memcpy(player->abilities, player->basic.abilities, Armor + 1);
}

static bool test_prerequisit(variant v) {
	if(v.iskind<abilityi>())
		return player->get((abilityn)v.value) >= v.value;
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
	auto m = player->getlevel();
	player->hpm = player->basic.hpm + imax(m, player->getbonus(Constitution) * m);
}

void creature::update() {
	pushvalue push_player(player, this);
	clear_creature();
	update_ability();
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
	enemy_id = 0xFFFF;
	setnoname();
}

void creature::add(classn v) {
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

int	creature::getrange(const creature* p) const {
	return iabs(abilities[Position] - p->abilities[Position]);
}

void creature::damage(int value) {
	if(value <= 0)
		return;
	auto push_player = player; player = this;
	actid(' ', "Damage", "Act", value);
	if(value > hp)
		value = hp;
	hp -= value;
	if(hp <= 0)
		actid(' ', "Die", "Act");
	actns(".");
	player = push_player;
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
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
		player->basic.abilities[i] = roll4d6m();
}

static void add_default_items() {
	auto id = player->getkind().getid();
	auto pi = bsdata<listi>::find(ids(id, "StartEquipment"));
	if(!pi)
		return;
	script_run(pi->id, pi->elements);
}

static void add_class_hit_points() {
	classi* pi = player->getkind();
	if(!pi)
		return;
	player->basic.hpm += pi->hd * 3;
}

static void finish() {
	player->hp = player->hpm;
}

void create_hero(int bonus) {
	player = bsdata<creature>::add();
	player->clear();
	player->setkind(bsdata<classi>::elements + last_class);
	player->setgender(last_gender);
	random_ability();
	player->add(last_class);
	add_class_hit_points();
	player->update();
	finish();
	add_default_items();
}

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gendern gender);

static void main_custom(stringbuilder& sb, const char* identifier) {
	if(stringvar_identifier(sb, identifier))
		return;
	if(apply_action(identifier, sb, player->getname(), player->getgender()))
		return;
	default_string(sb, identifier);
}

void initialize_printer() {
	stringbuilder::custom = main_custom;
}