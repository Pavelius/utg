#include "action.h"
#include "collection.h"
#include "creature.h"
#include "draw_utg.h"
#include "gender.h"
#include "itemlay.h"
#include "modifier.h"
#include "pushvalue.h"
#include "randomizer.h"
#include "roll.h"
#include "reaction.h"
#include "scenery.h"
#include "script.h"
#include "spell.h"

itema items;
spella spells;
static int critical_roll;
static void* last_option;

void generate_lair_treasure(const char* symbols);
void generate_treasure(const char* symbols, int group_count);

static int getbonus(int bonus) {
	switch(bonus) {
	case 101: return last_roll;
	case -101: return -last_roll;
	default: return bonus;
	}
}

template<> void fnscript<abilityi>(int index, int value) {
	switch(modifier) {
	case Permanent: player->basic.abilities[index] += getbonus(value); break;
	default: player->abilities[index] += getbonus(value); break;
	}
}

template<> void fnscript<itemi>(int index, int value) {
	auto pi = bsdata<itemi>::elements + index;
	item it(pi);
	player->equip(it);
	if(it)
		player->additem(it);
	if(pi->ammunition)
		fnscript<itemi>(pi->ammunition - bsdata<itemi>::elements, 0);
}

template<> void fnscript<feati>(int index, int value) {
	switch(modifier) {
	case Permanent: player->basic.feats.set(index, value >= 0); break;
	default: player->feats.set(index, value >= 0); break;
	}
}

template<> void fnscript<randomizeri>(int index, int value) {
	script_run(single(bsdata<randomizeri>::elements[index].random()));
}

template<> bool fntest<spelli>(int index, int value) {
	switch(modifier) {
	case NoModifier: return spell_effect((spell_s)index, player->get(Level), 0, false);
	default: return true;
	}
}
template<> void fnscript<spelli>(int index, int value) {
	switch(modifier) {
	case Known: player->known_spells.set(index); break;
	case NoModifier: spell_effect((spell_s)index, player->get(Level), 0, true); break;
	default: break;
	}
}

void choose_target() {
	opponent = targets.choose(getnm("ChooseTarget"), player->is(Enemy));
}

static int player_count() {
	auto result = 0;
	for(auto p : creatures) {
		if(p->is(Player) && p->get(HP) > 0)
			result++;
	}
	return result;
}

static void fix_action(const char* id) {
	player->actid(id, "Action");
}

static void clear_console() {
	if(answers::console)
		answers::console->clear();
}

static void damage_item() {
	if(last_item->damage())
		last_item->act(' ', getnm("ItemBroken"), last_item->getname());
}

static bool is_melee_fight() {
	feat_s feat = {};
	for(auto p : creatures) {
		if(!p->isready() || !p->is(EngageMelee))
			continue;
		feat_s t = {};
		if(p->is(Enemy))
			t = Enemy;
		else if(p->is(Player))
			t = Player;
		else
			continue;
		if(!feat)
			feat = t;
		else if(feat != t)
			return true;
	}
	return false;
}

static void update_melee_fight() {
	if(is_melee_fight())
		return;
	for(auto p : creatures)
		p->feats.remove(EngageMelee);
}

static void make_attack(const char* id, int bonus, ability_s attack, ability_s damage, wear_s weapon) {
	auto ac = opponent->get(AC);
	if(opponent->is(Small) && player->is(Large))
		ac += 2;
	last_item = player->wears + weapon;
	if(rolld20(bonus, 10 + ac, true)) {
		player->actid("Hit", id, ' ');
		int result = 0;
		if(critical_roll)
			result = last_item->getdamage().maximum();
		result += last_item->getdamage().roll();
		result += player->get(damage);
		opponent->damage(result);
		if(critical_roll && opponent->isready()) {
			pushvalue push_item(last_item, opponent->wears + Torso);
			damage_item();
		}
	} else {
		player->actid("Miss", id, ' ');
		if(critical_roll)
			damage_item();
	}
}

static void unarmed_attack(int bonus, interval damage) {
	auto ac = opponent->get(AC);
	if(rolld20(bonus, 10 + ac, true)) {
		player->actid("HitUnarmed", 0, ' ');
		int result = 0;
		if(critical_roll)
			result = damage.maximum();
		result += damage.roll();
		opponent->damage(result);
		if(critical_roll && opponent->isready()) {
			pushvalue push_item(last_item, opponent->wears + Torso);
			damage_item();
		}
	} else {
		player->actid("MissUnarmed", 0, ' ');
		if(critical_roll)
			player->set(Prone);
	}
}

static void melee_attack(int bonus) {
	make_attack("Melee", bonus, MeleeToHit, MeleeDamage, MeleeWeapon);
}

static void range_attack(int bonus) {
	make_attack("Ranged", bonus, RangedToHit, RangedDamage, RangedWeapon);
}

void select_enemies() {
	targets = creatures;
	targets.matchenemy(true);
	targets.matchready(true);
}

static int compare_initiative(const void* v1, const void* v2) {
	auto p1 = *((creature**)v1);
	auto p2 = *((creature**)v2);
	return p1->initiative - p2->initiative;
}

static void roll_initiative() {
	for(auto p : creatures)
		p->initiative = xrand(1, 6) + p->get(Speed);
	qsort(creatures.data, creatures.count, sizeof(creatures.data[0]), compare_initiative);
}

static void add_monsters(const monsteri* pm, int count, feat_s feat) {
	pushvalue push_player(player);
	for(auto i = 0; i < count; i++) {
		add_creature(pm);
		if(feat)
			player->set(feat);
	}
	if(count > 1)
		printn(getnm("AppearSeveral"), pm->getname());
	else
		printn(getnm("AppearSingle"), pm->getname());
}

static void random_encounter(const monsteri* pm) {
	if(!pm)
		return;
	add_monsters(pm, pm->dungeon.roll(), Enemy);
}

void random_encounter(const char* id) {
	random_encounter(bsdata<monsteri>::find(id));
}

static void random_melee_angry(int bonus) {
	select_enemies();
	targets.match(EngageMelee, false);
	zshuffle(targets.data, targets.count);
	if(bonus > (int)targets.count)
		bonus = targets.count;
	for(auto i = 0; i < bonus; i++)
		targets.data[i]->set(EngageMelee);
}

static void choose_target(int bonus) {
	choose_target();
}

static void choose_item() {
	if(player->is(Enemy))
		last_item = items.random();
	else
		last_item = items.choose(getnm("ChooseItem"));
}

static void enter_melee_combat() {
	if(!opponent)
		return;
	if(!opponent->is(EngageMelee) || !player->is(EngageMelee)) {
		player->set(EngageMelee);
		opponent->set(EngageMelee);
		random_melee_angry(1);
	}
}

static bool is_item_ready(wear_s type) {
	if(!player->wears[type] || player->wears[type].isbroken())
		return false;
	auto ammo = player->wears[type].geti().ammunition;
	if(ammo) {
		if(!player->wears[Ammunition])
			return false;
		if(&player->wears[Ammunition].geti() != ammo)
			return false;
	}
	return true;
}

static bool allow_attack_melee(int bonus) {
	if(!player->is(EngageMelee))
		return false;
	if(!is_item_ready(MeleeWeapon))
		return false;
	select_enemies();
	targets.match(EngageMelee, true);
	return targets.operator bool();
}

static void attack_melee(int bonus) {
	if(!allow_attack_melee(bonus))
		return;
	choose_target();
	enter_melee_combat();
	melee_attack(bonus);
}

static bool allow_attack_unarmed(int bonus) {
	if(is_item_ready(MeleeWeapon))
		return false;
	select_enemies();
	return targets.operator bool();
}

static void attack_unarmed(int bonus) {
	if(!allow_attack_unarmed(bonus))
		return;
	for(auto& e : player->attacks) {
		if(!e.count)
			continue;
		for(auto i = 0; i < e.count; i++) {
			select_enemies();
			if(!targets)
				return;
			choose_target();
			enter_melee_combat();
			unarmed_attack(0, e.damage);
		}
	}
}

static bool allow_attack_ranged(int bonus) {
	if(player->is(EngageMelee))
		return false;
	if(!is_item_ready(RangedWeapon))
		return false;
	select_enemies();
	return targets.operator bool();
}

static void attack_ranged(int bonus) {
	if(!allow_attack_ranged(bonus))
		return;
	choose_target();
	range_attack(0);
}

static bool allow_attack_charge(int bonus) {
	if(!is_item_ready(MeleeWeapon))
		return false;
	if(player->is(EngageMelee))
		return false;
	select_enemies();
	return targets.operator bool();
}

static void attack_charge(int bonus) {
	if(!allow_attack_charge(bonus))
		return;
	choose_target();
	enter_melee_combat();
	player->add(MeleeToHit, 1);
	player->add(AC, -1);
	melee_attack(0);
}

//static bool use_item(wear_s type, bool run) {
//	items.select(player->allitems());
//	items.match(type, true);
//	if(!items)
//		return false;
//	if(run) {
//		choose_item();
//		if(last_item) {
//			script_run(last_item->geti().use);
//			last_item->clear();
//		}
//	}
//	return true;
//}

static bool allow_retreat_melee(int bonus) {
	if(!player->is(EngageMelee))
		return false;
	creaturea source = creatures;
	source.matchally(true);
	source.matchyou(false);
	source.match(&creature::isready, true);
	source.match(EngageMelee, true);
	return source.operator bool();
}

static void retreat_melee(int bonus) {
	fix_action("Retreat");
	player->feats.remove(EngageMelee);
}

static void remove_player() {
	if(player->is(Summoned))
		player->clear();
	creatures.remove(player);
}

static bool allow_run_away(int bonus) {
	if(player->is(EngageMelee))
		return false;
	return true;
}

static void run_away(int bonus) {
	fix_action("RunAway");
	remove_player();
}

static void main_menu() {
}

static void surprise_roll(int bonus) {
	for(auto p : creatures) {
		auto result = d6();
		auto need_surprise = 2;
		if(result <= 2)
			p->set(Surprised);
	}
}

static void personal_experience(int bonus) {
	auto prime = player->get(player->geti().prime);
	if(prime >= 16)
		bonus = bonus * 110 / 100;
	else if(prime >= 13)
		bonus = bonus * 105 / 100;
	player->experience += bonus;
	player->levelup();
}

static void group_experience(int bonus) {
	if(bonus <= 0)
		return;
	auto count = player_count();
	if(!count)
		return;
	auto median = (bonus + count - 1) / count;
	printn(getnm("GroupExperienceAward"), bonus);
	pushvalue push(player);
	for(auto p : creatures) {
		if(p->is(Player) && p->get(HP) > 0) {
			player = p;
			personal_experience(median);
		}
	}
}

static void drop_loot(creature* p) {
	for(auto& e : p->allitems()) {
		if(!e)
			continue;
		if(e.iscursed() || e.ismagic() || (d100() < 30))
			e.drop();
	}
}

static void killed_enemy_loot() {
	auto reward = 0;
	pushvalue push(player);
	creaturea bodies;
	for(auto p : creatures) {
		if(p->is(Enemy) && p->get(HP) <= 0) {
			reward += p->getaward();
			drop_loot(p);
			bodies.add(p);
		}
	}
	// Separate collection `for each` is remove from `creatures` correctly.
	for(auto p : bodies)
		p->remove();
	group_experience(reward);
	// Add individual and group treasure
	generate_treasure("T", bodies.getcount());
}

static void add_items(const char* format) {
	for(auto p : items)
		an.add(p, format, p->getnamef());
}

static void take_item(item& ei) {
	for(auto p : creatures) {
		if(!p->is(Player))
			continue;
		p->additem(ei);
		if(!ei)
			break;
	}
}

static void take_items() {
	while(!draw::isnext() && items) {
		items.select(scene);
		an.clear();
		add_items(getnm("TakeItem"));
		last_option = an.choose(getnm("WhatToDoGroup"), getnm("LeaveThisItems"), 0);
		if(!last_option)
			break;
		else if(bsdata<itemlay>::have(last_option))
			take_item(*((item*)last_option));
	}
}

static void take_loot() {
	items.select(scene);
	if(!items) {
		prints(getnm("LootEnemyFails"));
		pause();
	} else {
		prints(getnm("LootEnemySuccess"));
		take_items();
	}
}

static bool allow_lose_game(int bonus) {
	targets = creatures;
	targets.match(Player, true);
	targets.match(&creature::isready, true);
	return !targets.operator bool();
}

static void lose_game(int bonus) {
	draw::setnext(main_menu);
}

static bool allow_win_battle(int bonus) {
	targets = creatures;
	targets.match(Enemy, true);
	targets.match(&creature::isready, true);
	return !targets.operator bool();
}

static void win_battle(int bonus) {
	clear_console();
	killed_enemy_loot();
	take_loot();
	draw::setnext(main_menu);
}

static bool allow_continue_battle(int bonus) {
	if(allow_win_battle(bonus) || allow_lose_game(bonus))
		return false;
	return true;
}

static void continue_battle(int bonus) {
	clear_console();
}

static void add_options(variant source) {
	if(source.iskind<listi>()) {
		auto pc = bsdata<listi>::elements + source.value;
		for(auto v : pc->elements) {
			if(!script_allow(v))
				continue;
			an.add(v.getpointer(), v.getname());
		}
	}
}

static void choose_option() {
	if(an)
		last_option = an.choose(0);
	else
		pause();
}

static void what_you_do(bool enemy_first_choose) {
	last_option = 0;
	if(!an)
		return;
	if(player->is(Enemy)) {
		if(enemy_first_choose)
			last_option = an.begin();
		else
			last_option = an.random();
	} else {
		char temp[260]; stringbuilder sb(temp);
		player->actv(sb, getnm("WhatToDo"), 0, 0);
		last_option = an.choose(temp);
	}
}

static void apply_scene_actions(variant v) {
	if(v.iskind<listi>()) {
		for(auto e : bsdata<listi>::elements[v.value].elements)
			apply_scene_actions(e);
	} else if(v.iskind<script>()) {
		auto p = bsdata<script>::elements + v.value;
		if(p->test && p->test(0))
			an.add(p, getnm(p->id));
	} else if(v.iskind<spelli>()) {
		auto p = bsdata<spelli>::elements + v.value;
		if(spell_effect((spell_s)v.value, player->get(Level), 0, false))
			an.add(p, p->getname());
	}
}

static void apply_scene_actions(int bonus) {
	if(bonus == 0)
		apply_scene_actions(scene->geti().actions);
}

static void apply_scene_spells(int bonus) {
	if(bonus == 0) {
		for(auto i = (spell_s)0; i <= LastSpell; i = (spell_s)(i + 1)) {
			if(!player->get(i))
				continue;
			auto p = bsdata<spelli>::elements + i;
			if(!player->cast(i, false))
				continue;
			an.add(p, getnm("CastSpell"), p->getname());
		}
	} else {
		if(bsdata<spelli>::have(last_option)) {
			auto spell = (spell_s)getbsi((spelli*)last_option); last_option = 0;
			player->cast(spell, true);
			player->use(spell);
		}
	}
}

static void apply_script_options() {
	if(bsdata<script>::have(last_option)) {
		auto p = last_option; last_option = 0;
		((script*)p)->proc(0);
	}
}

static void apply_option() {
	apply_script_options();
	apply_scene_spells(1);
}

void choose_options(variant source) {
	an.clear();
	add_options(source);
	choose_option();
	apply_option();
}

static void combat_round() {
	pushvalue push(player);
	for(auto p : creatures) {
		if(draw::isnext())
			break;
		if(!have_feats(Enemy) || !have_feats(Player))
			break;
		if(!p->isready())
			continue;
		player = p;
		if(player->is(Corrosed)) {
			player->damage(rollint(1, 3));
			if(!p->isready())
				continue;
		}
		if(player->is(CauseFear)) {
			player->feats.remove(EngageMelee);
			remove_player();
			continue;
		}
		if(player->is(Surprised)) {
			p->feats.remove(Surprised);
			continue;
		}
		player->update();
		an.clear();
		add_options("CombatRound");
		apply_scene_spells(0);
		what_you_do(true);
		apply_option();
		update_melee_fight();
	}
}

void combat_mode(int bonus) {
	auto push_mode = menu::current_mode;
	menu::current_mode = "Combat";
	roll_initiative();
	while(!draw::isnext() && allow_continue_battle(0)) {
		combat_round();
		choose_options("CombatTurn");
	}
	menu::current_mode = push_mode;
}

static void reaction_roll(int bonus) {
	if(player)
		bonus += player->getbonus(Charisma);
	auto r = d6() + d6() + bonus;
	if(r <= 2)
		reaction = Hostile;
	else if(r <= 5)
		reaction = Unfriendly;
	else if(r <= 8)
		reaction = Unfriendly;
	else if(r <= 11)
		reaction = Unfriendly;
	else
		reaction = Friendly;
}

static void all_saves(int bonus) {
	player->add(SaveDeath, bonus);
	player->add(SaveWands, bonus);
	player->add(SaveParalize, bonus);
	player->add(SaveBreathWeapon, bonus);
	player->add(SaveSpells, bonus);
}

static void undead_features(int bonus) {
	player->set(PoisonImmunity);
	player->set(Unholy);
}

static void select_allies(int bonus) {
	targets = creatures;
	if(player->is(Player))
		targets.match(Player, bonus >= 0);
	else if(player->is(Enemy))
		targets.match(Enemy, bonus >= 0);
}

static void select_items(int bonus) {
	items.select(scene);
}

static void select_backpack(int bonus) {
	items.select(player->backpack());
}

static void filter_alive(int bonus) {
	targets.match(&creature::isalive, bonus >= 0);
}

static void filter_broken(int bonus) {
	items.match(&item::isbroken, bonus >= 0);
}

static void filter_identified(int bonus) {
	items.match(&item::isidentified, bonus >= 0);
}

static void filter_cursed(int bonus) {
	items.match(&item::iscursed, bonus >= 0);
}

static void filter_you(int bonus) {
	targets.match(&creature::isplayer, bonus >= 0);
}

static void filter_wounded(int bonus) {
	targets.match(&creature::iswounded, bonus >= 0);
}

static bool targets_filter(int bonus) {
	last_script->proc(bonus);
	return targets.getcount();
}

static bool items_filter(int bonus) {
	last_script->proc(bonus);
	return targets.getcount();
}

static bool condition_passed(int bonus) {
	script_stop();
	return true;
}

static void heal(int bonus) {
}

static void for_each_creature(int bonus) {
	auto source = creatures;
	pushvalue push(player);
	variants commands = script_body();
	for(auto p : source) {
		player = p;
		script_run(commands);
	}
	script_stop();
}

static void hunt_prey(int bonus) {
}

static gender_s random_gender() {
	if(d100() < 40)
		return Female;
	return Male;
}

static int aligment_chance[] = {-1, 0, 1};

static int random_alignment() {
	return maprnd(aligment_chance);
}

static const classi* random_class(const char* id) {
	variant v = single(id);
	if(v.iskind<classi>())
		return bsdata<classi>::elements + v.value;
	return 0;
}

static int random_morale(int alignment) {
	switch(alignment) {
	case -1: return xrand(-30, -10);
	case 1: return xrand(10, 30);
	default: return xrand(-9, 9);
	}
}

static void add_character(const classi* pc, int level, int alignment, creature* leader = 0) {
	if(!pc)
		return;
	add_creature(pc, random_gender(), level);
	player->basic.abilities[Morale] = random_morale(alignment);
	player->setleader(leader);
}

static void add_followers(const classi* pc, int alignment, interval count, interval level) {
	auto n = count.roll();
	auto leader = player;
	pushvalue push_player(player);
	for(auto i = 0; i < n; i++)
		add_character(pc, level.roll(), alignment, leader);
}

static void add_followers(const char* class_id, int alignment, interval count, interval level) {
	auto n = count.roll();
	auto leader = player;
	pushvalue push_player(player);
	for(auto i = 0; i < n; i++)
		add_character(random_class(class_id), level.roll(), alignment, leader);
}

static void cleric_high_level(int bonus) {
	auto cleric_class = bsdata<classi>::find("Cleric");
	auto fighter_class = bsdata<classi>::find("Fighter");
	if(!cleric_class || !fighter_class)
		return;
	pushvalue push(player);
	auto alignment = random_alignment();
	add_character(cleric_class, d6() + 6, alignment);
	if(!player)
		return;
	add_followers(cleric_class, alignment, {1, 4}, {2, 5});
	add_followers(fighter_class, alignment, {1, 3}, {1, 6});
}

static void fighter_high_level(int bonus) {
	auto fighter_class = bsdata<classi>::find("Fighter");
	if(!fighter_class)
		return;
	pushvalue push(player);
	auto alignment = random_alignment();
	add_character(fighter_class, d4() + 6, alignment);
	if(!player)
		return;
	add_followers("RandomClass", alignment, {2, 8}, {3, 6});
}

static void magic_user_high_level(int bonus) {
	auto magic_user_class = bsdata<classi>::find("MagicUser");
	auto fighter_class = bsdata<classi>::find("Fighter");
	if(!magic_user_class || !fighter_class)
		return;
	pushvalue push(player);
	auto alignment = random_alignment();
	add_character(magic_user_class, d4() + 6, alignment);
	if(!player)
		return;
	add_followers(magic_user_class, alignment, {1, 4}, {1, 3});
	add_followers(fighter_class, alignment, {1, 4}, {2, 5});
}

static void adventurers_basic(int bonus) {
	pushvalue push(player); player = 0;
	add_followers("RandomClass", random_alignment(), {5, 8}, {1, 3});
}

static void adventurers_expert(int bonus) {
	pushvalue push(player); player = 0;
	add_followers("RandomClass", random_alignment(), {4, 9}, {3, 8});
}

static void apply_leader() {
	for(auto p : encountered) {
		if(!p->getleader())
			p->setleader(player);
	}
}

static void make_leader(int bonus) {
	player = encountered.random();
	player->basic.abilities[HP] = player->get(Level) * player->geti().hd;
	player->basic.abilities[MeleeDamage] += bonus;
	player->basic.abilities[RangedDamage] += bonus;
	apply_leader();
}

static void random_level3(int bonus) {
	player->basic.abilities[Level] = rollint(3, bonus);
}

static void acid_damage(int bonus) {
	opponent->set(Corrosed);
}

BSDATA(script) = {
	{"AcidDamage", acid_damage},
	{"AdventurersBasic", adventurers_basic},
	{"AdventurersExpert", adventurers_expert},
	{"AttackCharge", attack_charge, allow_attack_charge},
	{"AttackMelee", attack_melee, allow_attack_melee},
	{"AttackRanged", attack_ranged, allow_attack_ranged},
	{"AttackUnarmed", attack_unarmed, allow_attack_unarmed},
	{"ChooseTarget", choose_target, condition_passed},
	{"ClericHightLevel", cleric_high_level},
	{"CombatMode", combat_mode},
	{"ContinueBattle", continue_battle, allow_continue_battle},
	{"FighterHightLevel", fighter_high_level},
	{"FilterAlive", filter_alive, targets_filter},
	{"FilterBroken", filter_cursed, items_filter},
	{"FilterCursed", filter_cursed, items_filter},
	{"FilterIdentified", filter_identified, items_filter},
	{"FilterYou", filter_you, targets_filter},
	{"FilterWounded", filter_wounded, targets_filter},
	{"ForEachCreature", for_each_creature, targets_filter},
	{"HuntPrey", hunt_prey},
	{"LoseGame", lose_game, allow_lose_game},
	{"MagicUserHightLevel", magic_user_high_level},
	{"MakeLeader", make_leader},
	{"RandomLevel3", random_level3},
	{"ReactionRoll", reaction_roll},
	{"RetreatMelee", retreat_melee, allow_retreat_melee},
	{"RunAway", run_away, allow_run_away},
	{"Saves", all_saves},
	{"SelectAllies", select_backpack, targets_filter},
	{"SelectBackpack", select_backpack, items_filter},
	{"SelectItems", select_items, items_filter},
	{"SurpriseRoll", surprise_roll},
	{"Undead", undead_features},
	{"WinBattle", win_battle, allow_win_battle},
};
BSDATAF(script)