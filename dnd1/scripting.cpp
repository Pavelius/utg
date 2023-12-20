#include "action.h"
#include "collection.h"
#include "creature.h"
#include "draw_utg.h"
#include "gender.h"
#include "modifier.h"
#include "pushvalue.h"
#include "randomizer.h"
#include "roll.h"
#include "reaction.h"
#include "scenery.h"
#include "script.h"
#include "spell.h"

creaturea targets;
itema items;
spella spells;
static int critical_roll;

template<> void fnscript<abilityi>(int index, int value) {
	switch(modifier) {
	case Permanent: player->basic.abilities[index] += value; break;
	default: player->abilities[index] += value; break;
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

static int player_count() {
	auto result = 0;
	for(auto p : creatures) {
		if(p->is(Player) && p->get(HP) > 0)
			result++;
	}
	return result;
}

static void fix_action() {
	player->actid(last_option->id, "Action");
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

static bool have_feats(feat_s v) {
	for(auto p : creatures) {
		if(!p->isready())
			continue;
		if(p->is(v))
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

void choose_target() {
	opponent = targets.choose(getnm("ChooseTarget"), player->is(Enemy));
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

static bool attack_melee(bool run) {
	if(!player->is(EngageMelee))
		return false;
	if(!is_item_ready(MeleeWeapon))
		return false;
	select_enemies();
	targets.match(EngageMelee, true);
	if(!targets)
		return false;
	if(run) {
		choose_target();
		enter_melee_combat();
		melee_attack(0);
	}
	return true;
}

static bool attack_unarmed(bool run) {
	if(is_item_ready(MeleeWeapon))
		return false;
	select_enemies();
	if(!targets)
		return false;
	if(run) {
		for(auto& e : player->attacks) {
			if(!e.count)
				continue;
			for(auto i = 0; i < e.count; i++) {
				select_enemies();
				if(!targets)
					return true;
				choose_target();
				enter_melee_combat();
				unarmed_attack(0, e.damage);
			}
		}
	}
	return true;
}

static bool attack_range(bool run) {
	if(player->is(EngageMelee))
		return false;
	if(!is_item_ready(RangedWeapon))
		return false;
	select_enemies();
	if(!targets)
		return false;
	if(run) {
		choose_target();
		range_attack(0);
	}
	return true;
}

static bool charge(bool run) {
	if(!is_item_ready(MeleeWeapon))
		return false;
	if(player->is(EngageMelee))
		return false;
	select_enemies();
	if(!targets)
		return false;
	if(run) {
		choose_target();
		enter_melee_combat();
		player->add(MeleeToHit, 1);
		player->add(AC, -1);
		melee_attack(0);
	}
	return true;
}

static bool use_item(wear_s type, bool run) {
	items.select(player->allitems());
	items.match(type, true);
	if(!items)
		return false;
	if(run) {
		choose_item();
		if(last_item) {
			script_run(last_item->geti().use);
			last_item->clear();
		}
	}
	return true;
}

static bool drink_potion(bool run) {
	return use_item(Potion, run);
}

static bool read_scroll(bool run) {
	return use_item(Scroll, run);
}

static bool prepare_spells(bool run) {
	spells.select(player->known_spells);
	if(!spells)
		return false;
	if(run) {

	}
	return true;
}

static void choose_spell() {
	last_spell = (spell_s)(spells.choose(getnm("ChooseSpellToCast")) - bsdata<spelli>::elements);
}

static bool cast_spells(bool run) {
	spells.select(*player);
	if(!spells)
		return false;
	if(run) {
		choose_spell();
		//player->cast(last_spell);
	}
	return true;
}

static bool retreat_melee(bool run) {
	if(!player->is(EngageMelee))
		return false;
	creaturea source = creatures;
	source.matchally(true);
	source.matchyou(false);
	source.match(&creature::isready, true);
	source.match(EngageMelee, true);
	if(!source)
		return false;
	if(run) {
		fix_action();
		player->feats.remove(EngageMelee);
	}
	return true;
}

static void remove_player() {
	if(player->is(Summoned))
		player->clear();
	creatures.remove(player);
}

static bool run_away(bool run) {
	if(player->is(EngageMelee))
		return false;
	if(run) {
		fix_action();
		remove_player();
	}
	return true;
}

static chooseoption camp_options[] = {
	{"PrepareSpells", prepare_spells},
};

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

static void combat_round() {
	static chooseoption options[] = {
		{"AttackRanged", attack_range},
		{"ChargeEnemy", charge},
		{"AttackMelee", attack_melee},
		{"AttackUnarmed", attack_unarmed},
		{"DrinkPotion", drink_potion},
		{"ReadScroll", read_scroll},
		{"Retreat", retreat_melee},
		{"RunAway", run_away},
	};
	for(auto p : creatures) {
		if(draw::isnext())
			break;
		if(!have_feats(Enemy) || !have_feats(Player))
			break;
		if(!p->isready())
			continue;
		if(p->is(Panic)) {
			p->feats.remove(Panic);
			p->feats.remove(EngageMelee);
			p->use("RunAway", options);
			continue;
		}
		if(p->is(Surprised)) {
			p->feats.remove(Surprised);
			continue;
		}
		p->update();
		p->choose(options, true);
		update_melee_fight();
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

static void killed_enemy_loot(int bonus) {
	auto reward = 0;
	pushvalue push(player);
	for(auto p : creatures) {
		if(p->is(Enemy) && p->get(HP) <= 0) {
			reward += p->getaward();
			drop_loot(p);
			p->remove();
		}
	}
	group_experience(reward);
}

static bool lose_game(bool run) {
	targets = creatures;
	targets.match(Player, true);
	targets.match(&creature::isready, true);
	if(targets)
		return false;
	if(run)
		draw::setnext(main_menu);
	return true;
}

static bool win_battle(bool run) {
	targets = creatures;
	targets.match(Enemy, true);
	targets.match(&creature::isready, true);
	if(targets)
		return false;
	if(run) {
		killed_enemy_loot(0);
		pause();
		draw::setnext(main_menu);
	}
	return true;
}

static bool continue_battle(bool run) {
	if(win_battle(false) || lose_game(false))
		return false;
	if(run) {
		clear_console();
	}
	return true;
}

void combat_mode() {
	static chooseoption options[] = {
		{"LoseGame", lose_game},
		{"WinBattle", win_battle},
		{"ContinueBattle", continue_battle},
	};
	auto push_mode = menu::current_mode;
	menu::current_mode = "Combat";
	roll_initiative();
	while(!draw::isnext() && continue_battle(false)) {
		combat_round();
		choose(options, 0);
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

BSDATA(script) = {
	{"ChooseTarget", choose_target, condition_passed},
	{"FilterAlive", filter_alive, targets_filter},
	{"FilterBroken", filter_cursed, items_filter},
	{"FilterCursed", filter_cursed, items_filter},
	{"FilterIdentified", filter_identified, items_filter},
	{"FilterYou", filter_you, targets_filter},
	{"FilterWounded", filter_wounded, targets_filter},
	{"ReactionRoll", reaction_roll},
	{"Saves", all_saves},
	{"SelectAllies", select_backpack, targets_filter},
	{"SelectBackpack", select_backpack, items_filter},
	{"SelectItems", select_items, items_filter},
	{"SurpriseRoll", surprise_roll},
	{"Undead", undead_features},
};
BSDATAF(script)