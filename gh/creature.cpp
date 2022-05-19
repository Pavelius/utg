#include "main.h"

using namespace pathfind;

BSDATAC(creaturei, 128)

static creaturei* addnew() {
	for(auto& e : bsdata<creaturei>()) {
		if(!e)
			return &e;
	}
	return bsdata<creaturei>::add();
}

static monsteri* find_monster(const char* id, char elite) {
	id = szdup(id);
	for(auto& e : bsdata<monsteri>()) {
		if(e.elite == elite && e.level == game.dungeon_level && e.id == id)
			return &e;
	}
	return 0;
}

creaturei* creaturei::add(const char* id, point position, bool elite) {
	auto p = addnew();
	p->clear();
	if(!p->parent) {
		auto pc = bsdata<playeri>::find(id);
		if(pc) {
			p->parent = pc;
			p->level = pc->level;
		}
	}
	if(!p->parent) {
		auto pc = bsdata<summoni>::find(id);
		if(pc) {
			p->parent = pc;
			p->level = game.dungeon_level;
		}
	}
	if(!p->parent) {
		auto pc = find_monster(id, elite ? 1 : 0);
		if(pc) {
			p->parent = pc;
			p->level = game.dungeon_level;
		}
	}
	p->hits = p->getmaximumhp();
	p->setposition(position);
	p->updateui();
	return p;
}

void creaturei::clear() {
	memset(this, 0, sizeof(*this));
}

playeri* creaturei::getplayer() const {
	return bsdata<playeri>::have(parent) ? (playeri*)parent : 0;
}

const summoni* creaturei::getmonster() const {
	if(bsdata<summoni>::have(parent) || bsdata<monsteri>::have(parent))
		return (summoni*)parent;
	return 0;
}

int creaturei::getmaximumhp() const {
	auto pc = getplayer();
	if(pc)
		return pc->health[pc->level];
	auto pm = getmonster();
	if(pm)
		return pm->hits;
	return 0;
}

void creaturei::kill() {
	fixkill();
	clear();
}

void creaturei::damage(int v) {
	if(v <= 0)
		return;
	fixdamage(v);
	hits -= v;
	if(hits <= 0) {
		hits = 0;
		kill();
	}
}

int creaturei::getongoing(action_s id) const {
	auto result = 0;
	for(auto& e : bsdata<activecardi>()) {
		if(!e || e.target != this)
			continue;
		e.use();
	}
	return result;
}

void creaturei::attack(creaturei& enemy, int bonus, int pierce) {
	if(enemy.is(Poison))
		bonus += 1;
	auto& deck = getcombatdeck();
	auto next = 1;
	auto need_shuffle = false;
	while(next-- > 0) {
		combatcardi* p;
		if(is(Strenght))
			p = deck.takegood(2);
		else if(is(Muddle))
			p = deck.takebad(2);
		else
			p = deck.take();
		if(p->bonus == -100) {
			bonus = -100;
			break; // Miss
		} else if(p->bonus == 100)
			bonus *= 2;
		else
			bonus += p->bonus;
		next += p->next;
		if(p->shuffle)
			need_shuffle = true;
		deck.discard(p);
	}
	if(need_shuffle)
		deck.shuffle();
	if(bonus > 0) {
		auto shield = enemy.getongoing(Shield) - pierce;
		if(shield < 0)
			shield = 0;
		bonus -= shield;
	}
	fixattack(enemy);
	enemy.damage(bonus);
}

combatdeck&	creaturei::getcombatdeck() const {
	if(isplayer())
		return ((playeri*)parent)->combat;
	return game.combat;
}

bool creaturei::isplayer() const {
	return bsdata<playeri>::have(parent);
}

const char* creaturei::getid() const {
	if(bsdata<monsteri>::have(parent) || bsdata<summoni>::have(parent))
		return ((summoni*)parent)->id;
	if(bsdata<playeri>::have(parent))
		return ((playeri*)parent)->id;
	return 0;
}

static void blockwalls() {
	for(auto i = 0; i < hms * hms; i++) {
		if(game.iswall(i))
			setmove(i, Blocked);
	}
}

static void block(indext i) {
	if(i != Blocked)
		setmove(i, Blocked);
}

static void blockcreatures() {
	for(auto& e : bsdata<creaturei>()) {
		if(!e)
			continue;
		block(e.getindex());
	}
}

static void blockcreatures(bool you_is_hostile) {
	for(auto& e : bsdata<creaturei>()) {
		if(!e)
			continue;
		if(e.is(Hostile) == you_is_hostile)
			continue;
		block(e.getindex());
	}
}

static void calculate_movemap(indext start, int range, bool you_is_hostile, bool jump, bool fly) {
	clearpath();
	blockwalls();
	if(jump || fly)
		makewave(start);
	else {
		blockcreatures(you_is_hostile);
		makewave(start);
	}
	blockcreatures();
	blockrange(range);
}

static void calculate_shootmap(indext start) {
	clearpath();
	blockwalls();
	makewave(start);
}

void creaturei::move(int bonus) {
	while(bonus > 0) {
		calculate_movemap(getindex(), bonus, is(Hostile), is(Jump), is(Fly));
		auto new_index = choosemove();
		if(new_index == Blocked)
			break;
		setposition(new_index);
		bonus -= getmove(new_index);
		fixmove(i2h(new_index));
	}
}

int	creaturei::getinitiative(int index) const {
	auto p = getplayer();
	if(p && p->cards[index])
		return p->cards[index]->initiative;
	return 99;
}

void creaturei::play() {
	auto p = getplayer();
	if(!p)
		return;
	bool use_card_1 = false;
	bool use_card_2 = false;
	bool use_upper = false;
	bool use_lower = false;
	for(auto i = 0; i < 2; i++) {
		answers an;
		if(!use_card_1)
			an.add(p->cards[0], getnm(p->cards[0]->id));
		if(!use_card_2)
			an.add(p->cards[1], getnm(p->cards[1]->id));
		auto pc = (playercardi*)an.choose(0, 0, 1);
		if(pc == p->cards[0])
			use_card_1 = true;
		if(pc == p->cards[1])
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
		apply(*pv);
	}
}

void creaturei::choosecards() {
	auto p = getplayer();
	if(!p)
		return;
	p->cards[0] = p->hand.choose(0, true);
	p->cards[1] = p->hand.choose(0, true);
	chooseinitiative();
}

void creaturei::chooseinitiative() {
	auto p = getplayer();
	if(!p || !p->cards[0] || !p->cards[1])
		return;
	const char* format = getnm("InitiativeOrder");
	answers an;
	an.add((void*)0, format, p->cards[0]->initiative);
	an.add((void*)1, format, p->cards[1]->initiative);
	auto result = an.choose(getnm("ChooseInitiative"));
	if(result)
		iswap(p->cards[0], p->cards[1]);
}

creaturei* creaturei::chooseenemy() const {
	return choosenearest(!is(Hostile));
}

creaturei* creaturei::choosenearest(bool hostile) const {
	creaturea targets;
	targets.select();
	targets.match(Hostile, hostile);
	return targets.choose(0);
}

void creaturei::addexperience(int value) {
	if(value <= 0)
		return;
	auto p = getplayer();
	if(!p)
		return;
	fixexperience(value);
	p->exp += value;
}

void creaturei::activate() {
	game.focusing(getposition());
}