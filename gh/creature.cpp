#include "main.h"

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

const playeri* creaturei::getplayer() const {
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
		if(!e)
			continue;
		auto used = false;
		for(auto v : e.effect) {
			if(!v.iskind<actioni>())
				continue;
			if(v.value != id)
				continue;
			result += v.counter;
			used = true;
		}
		if(used)
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

void creaturei::move(int bonus) {

}