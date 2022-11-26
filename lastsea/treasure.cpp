#include "main.h"

bool treasurei::isdiscardable() const {
	return tags.is(Discard);
}

void treasurei::triggered() {
	switch(trigger) {
	case WhenUse:
	case WhenChooseSpecial:
		apply();
		break;
	case WhenRoll:
		last_bonus += bonus;
		break;
	}
	if(isdiscardable())
		game.losstreasure(this);
}

void treasurei::prepare() {
	for(auto& e : bsdata<treasurei>()) {
		e.tags.remove(Discarded);
		e.owner = 0xFFFF;
	}
}

treasurei* treasurei::pickvaluable() {
	collectiona source;
	for(auto& e : bsdata<treasurei>()) {
		if(e.isactive() || e.is(Discarded) || !e.is(Valuable))
			continue;
		source.add(&e);
	}
	return (treasurei*)source.random();
}

void treasurei::setowner(const void* object) {
	if(!object) {
		if(owner != 0xFFFF) {
			game.warning(getnm("LoseTreasure"), getnm(id));
			lossing();
		}
		owner = 0xFFFF;
	} else {
		if(owner == 0xFFFF) {
			game.information(getnm("GainTreasure"), getnm(id));
			gaining();
		}
		owner = 1;
	}
}

treasurei* treasurei::find(const char* id) {
	if(!id)
		return 0;
	for(auto& e : bsdata<treasurei>()) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

treasurei* treasurei::pickstory(int number) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("Story%1i", number);
	return find(temp);
}

treasurei* treasurei::pickvaluable(int number) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("Treasure%1i", number);
	return find(temp);
}

void treasurei::gaining() const {
	game.apply(gain);
}

void treasurei::lossing() const {
	game.apply(loss);
}

void treasurei::apply() const {
	game.apply(use);
}