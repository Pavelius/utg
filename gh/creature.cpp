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
	clear();
}

void creaturei::damage(int v) {
	if(v <= 0)
		return;
	hits -= v;
	if(hits <= 0) {
		hits = 0;
		kill();
	}
}

void creaturei::attack(creaturei& enemy, int bonus) {
	auto& deck = getcombatdeck();
	auto next = 1;
	auto need_shuffle = false;
	while(next--) {
		auto p = deck.take();
		if(p->bonus == -100)
			return; // Miss
		else if(p->bonus == 100)
			bonus *= 2;
		else
			bonus += p->bonus;
		if(p->shuffle)
			need_shuffle = true;
		deck.discard(p);
	}
	if(need_shuffle)
		deck.shuffle();
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