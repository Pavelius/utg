#include "main.h"

static creaturea creatures;

static bool if_element(int bonus, int param) {
	return false;
}

static bool if_adjanced_ally(int bonus, int param) {
	return false;
}

static bool if_not_adjanced_his_ally(int bonus, int param) {
	return false;
}

static bool if_state(int bonus, int param) {
	return false;
}

void initialize_conditions() {
	conditioni::add("IfAir", if_element, Air);
	conditioni::add("IfDark", if_element, Dark);
	conditioni::add("IfEarth", if_element, Earth);
	conditioni::add("IfFire", if_element, Fire);
	conditioni::add("IfIce", if_element, Ice);
	conditioni::add("IfInvisibility", if_state, Invisibility);
	conditioni::add("IfLight", if_element, Light);
	conditioni::add("IfTargetAdjancedAlly", if_adjanced_ally);
	conditioni::add("IfTargetNotAdjancedHisAlly", if_not_adjanced_his_ally);
}

static void double_attack(int bonus) {
}

static void suffer_damage(int bonus) {
}

static void recover_discarded(int bonus) {
}

static void play_player_turn() {
	bool use_card_1 = false;
	bool use_card_2 = false;
	bool use_upper = false;
	bool use_lower = false;
	for(auto i = 0; i < 2; i++) {
		an.clear();
		if(!use_card_1)
			an.add(player->cards[0], getnm(player->cards[0]->id));
		if(!use_card_2)
			an.add(player->cards[1], getnm(player->cards[1]->id));
		auto pc = (playercardi*)an.choose(0, 0, 1);
		if(pc == player->cards[0])
			use_card_1 = true;
		if(pc == player->cards[1])
			use_card_2 = true;
		an.clear();
		if(!use_upper)
			an.add(&pc->upper, getnm("UpperCardPart"));
		if(!use_lower)
			an.add(&pc->lower, getnm("LowerCardPart"));
		auto pv = (variants*)an.choose(0, 0, 1);
		if(pv == &pc->upper)
			use_upper = true;
		if(pv == &pc->lower)
			use_lower = true;
		playercardi::last = pc;
		active_creature->apply(*pv);
	}
}

static void play_monster_turn() {
	auto p = active_creature->getmonster();
	if(!p)
		return;
	auto pc = monsterdeck::get(p->getdeck()).current();
	if(pc)
		active_creature->apply(pc->abilities);
}

static void play_moves() {
	auto push_player = player;
	auto push_active = active_creature;
	creatures.clear();
	creatures.select();
	creatures.sort();
	for(auto p : creatures) {
		if(!(*p))
			continue;
		p->activate();
		player = p->getplayer();
		if(player)
			play_player_turn();
		else
			play_monster_turn();
	}
	active_creature = push_active;
	player = push_player;
}

static void check_initiative() {
}

static void choose_initiative() {
	if(!player->cards[0] || !player->cards[1])
		return;
	const char* format = getnm("InitiativeOrder");
	answers an;
	an.add((void*)0, format, player->cards[0]->initiative);
	an.add((void*)1, format, player->cards[1]->initiative);
	auto result = an.choose(getnm("ChooseInitiative"));
	if(result)
		iswap(player->cards[0], player->cards[1]);
}

static void choose_battle_cards() {
	player->cards[0] = player->hand.choose(0, true);
	player->cards[1] = player->hand.choose(0, true);
}

static void choose_cards() {
	auto push_player = player;
	auto push_active = active_creature;
	for(auto& e : bsdata<creaturei>()) {
		if(!e)
			continue;
		player = e.getplayer();
		if(!player)
			continue;
		e.activate();
		choose_battle_cards();
		choose_initiative();
	}
	active_creature = push_active;
	player = push_player;
}

void gamei::playround() {
	choose_cards();
	check_initiative();
	play_moves();
}

BSDATA(script) = {
	{"AttackDouble", double_attack},	
	{"RecoverDiscarded", recover_discarded},
	{"SufferDamage", suffer_damage},
};
BSDATAF(script)