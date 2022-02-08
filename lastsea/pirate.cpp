#include "main.h"

void pirate::clearactions() {
	memset(actions, 0, sizeof(actions));
}

void pirate::clear() {
	memset(this, 0, sizeof(*this));
	clearactions();
	abilities[MissionMaximum] = 5;
	abilities[CabineMaximum] = 5;
}

int pirate::getnextstar(int value) const {
	auto& ei = getclass();
	auto old_value = 0;
	for(auto v : ei.levelup) {
		if(value >= old_value && value < v)
			return v;
	}
	return 0;
}

void pirate::sfgetproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (pirate*)object;
	int value, value2;
	switch(v.type) {
	case Ability:
		switch(v.value) {
		case Stars:
			value = p->get((ability_s)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((ability_s)v.value));
			value = p->getnextstar(value);
			if(value)
				sb.adds("(%1 %2i)", getnm("NextStar"), value);
			break;
		case Crew:
			value = p->get((ability_s)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((ability_s)v.value));
			value = p->get(Discontent);
			if(value)
				sb.adds(getnm("ShowDiscontent"), value);
			break;
		case Reroll: case Misfortune:
			value = p->get((ability_s)v.value);
			if(value)
				sb.add("%1i", value);
			break;
		default:
			value = p->get((ability_s)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((ability_s)v.value));
			value2 = p->getbonus((ability_s)v.value);
			if(value2)
				sb.adds("(%-IncludeItems %1i)", value + value2);
			break;
		}
		break;
	}
}

const char* pirate::getavatarst(const void* object) {
	auto p = (pirate*)object;
	auto i = bsdata<pirate>::source.indexof(p);
	if(i == -1)
		return 0;
	static char temp[260];
	stringbuilder sb(temp);
	sb.add("pirate%1i", i + 1);
	return temp;
}

bool pirate::match(variant v) const {
	switch(v.type) {
	case Gender: return getgender() == v.value;
	default:
		return false;
	}
}

int	pirate::getmaximum(ability_s v) const {
	const goali* p;
	switch(v) {
	case Exploration:
	case Brawl:
	case Hunting:
	case Aim:
	case Swagger:
	case Navigation:
		return getclass().maximum[v - Exploration];
	case Infamy:
		return 6;
	case Stars: return 18;
	case Level: case Mission: case Cabine: case Threat:
		return 5;
	case Danger:
		p = game.getgoal();
		if(p)
			return p->danger;
		return 10;
	default: return 10;
	}
}

const char* classi::getearn(ability_s v) const {
	switch(v) {
	case Exploration: return exploration;
	case Brawl: return brawl;
	case Hunting: return hunting;
	case Aim: return aim;
	case Swagger: return swagger;
	case Navigation: return navigation;
	default: return 0;
	}
}

void pirate::checkexperience(ability_s v) {
	auto pa = getclass().getearn(v);
	auto cv = get(v);
	for(unsigned i = 0; i < sizeof(classi::aim) / sizeof(classi::aim[0]); i++) {
		if(cv == pa[i]) {
			set(Stars, get(Stars) + 1);
			break;
		}
	}
}

void pirate::set(ability_s v, int i) {
	if(i < 0)
		i = 0;
	if(v == Stars) {
		v = Infamy;
		i = abilities[Infamy] + (i - abilities[Stars]) * 3;
		if(i < 0)
			i = 0;
	}
	auto m = getmaximum(v);
	if(i > m) {
		if(v == Infamy) {
			game.information(getnm("YouGainStars"));
			abilities[Infamy] = 0;
			if(abilities[Stars] < getmaximum(Stars))
				abilities[Stars]++;
			return;
		} else
			i = m;
	}
	if(abilities[v] != i) {
		auto d = i - abilities[v];
		switch(v) {
		case Mission: case Cabine: case Threat:
			abilities[v] = i;
			break;
		default:
			if(v >= Exploration && v <= Navigation) {
				if(!confirm(v, d))
					return;
				set(Supply, get(Supply) - d);
			}
			if(v >= 0 && v <= Infamy) {
				game.information(v, d);
				abilities[v] = i;
			}
			break;
		}
		afterchange(v, d);
	}
}

void pirate::addaction(indext v) {
	for(auto& e : actions) {
		if(e)
			continue;
		e = v;
		break;
	}
}

void pirate::gaintreasures(int count) {
	if(count > 0) {
		for(auto i = 0; i < count; i++) {
			if(gettreasurecount(Valuable) >= max_treasures) {
				set(Supply, get(Supply) + (count - i));
				return;
			}
			auto pv = treasurei::pickvaluable();
			if(pv)
				gaintreasure(pv);
		}
	} else {
		count = -count;
		for(auto i = 0; i < count; i++) {
			auto pv = choosetreasure(0, 0);
			if(pv)
				losstreasure(pv);
		}
	}
}

bool pirate::confirm(ability_s v, int delta) const {
	if(delta <= 0)
		return true;
	if(get(Supply) < delta)
		return false;
	return true;
}

static int compare(const void* v1, const void* v2) {
	auto e1 = *((indext*)v1);
	auto e2 = *((indext*)v2);
	auto p1 = (e1 == 0) ? 30000 : e1;
	auto p2 = (e2 == 0) ? 30000 : e2;
	return p1 - p2;
}

void pirate::sortactions() {
	qsort(actions, sizeof(actions) / sizeof(actions[0]), sizeof(actions[0]), compare);
}

ability_s pirate::chooseskill(const char* title) const {
	answers an;
	for(auto i = Exploration; i <= Navigation; i = (ability_s)(i + 1)) {
		if(get(i) >= getmaximum(i))
			continue;
		auto& ei = bsdata<abilityi>::elements[i];
		an.add(&ei, getnm(ei.id));
	}
	return (ability_s)bsdata<abilityi>::source.indexof(an.choose(title));
}

static const quest* find_next_action(const quest* p) {
	if(p && p[1].index == p[0].index && p[1].next > 100)
		return p + 1;
	return 0;
}

void pirate::rolldices() {
	rollv(last_bonus);
	int last_second_roll = 0;
	if(get(Misfortune) > 0) {
		abilities[Misfortune]--;
		last_second_roll = 1 + rand() % 12;
		if(last_roll > last_second_roll) {
			iswap(last_second_roll, last_roll);
			last_result = last_roll + last_bonus;
		}
		if(last_result < 1)
			last_result = 1;
		game.information(getnm("YouRollMisfortune"), last_result, last_roll, last_bonus, last_second_roll);
	} else {
		if(last_result < 1)
			last_result = 1;
		game.information(getnm("YouRoll"), last_result, last_roll, last_bonus, last_second_roll);
	}
}

void pirate::confirmroll() {
	answers an;
	auto push_text = utg::sb.get();
	while(true) {
		rolldices();
		an.clear();
		an.add(0, getnm("ApplyRoll"));
		if(get(Reroll) > 0)
			an.add(&bsdata<abilityi>::get(Reroll), getnm("UseReroll"));
		auto pv = an.choose(0);
		if(!pv)
			break;
		if(bsdata<abilityi>::have(pv)) {
			switch(bsdata<abilityi>::source.indexof(pv)) {
			case Reroll:abilities[Reroll]--; break;
			default: break;
			}
		}
		utg::sb.set(push_text);
	}
	last_bonus = 0;
}

static const quest* find_action_choose(const quest* ph) {
	if(!ph)
		return 0;
	const quest* p = ph + 1;
	while(p->index == ph->index) {
		if(p->next == 0)
			return p;
		p++;
	}
	return 0;
}

void pirate::bury(int count) {
	char temp[260]; stringbuilder sb(temp);
	while(count--) {
		sb.clear();
		sb.add(getnm("WhatTreasureToBury"));
		if(count > 1)
			sb.adds(getnm("ChooseLeft"), count);
		auto p = choosetreasure(temp, getnm("StopBury"));
		if(!p)
			break;
		losstreasure(p);
		set(Stars, get(Stars) + 1);
	}
}

void pirate::choosebonus(variant v1, variant v2) {
	answers an;
	an.add(&v1, "%Get %1%2i", getnm(bsdata<abilityi>::elements[v1.value].id), v1.counter);
	an.add(&v2, "%Get %1%2i", getnm(bsdata<abilityi>::elements[v2.value].id), v2.counter);
	auto pv = (variant*)an.choose();
	if(pv)
		game.apply(*pv);
}

void pirate::tradefriend() {
	auto p = choosetreasure(getnm("TradeTreasure"), getnm("Cancel"));
	if(!p)
		set(Reroll, get(Reroll) + 1);
	else {
		losstreasure(p);
		gaintreasure(treasurei::pickvaluable());
	}
}

void pirate::raiseskills(int count) {
	char temp[260]; stringbuilder sb(temp);
	while(count > 0) {
		sb.clear(); sb.add(getnm("ChooseSkillToRaise"));
		if(count > 1)
			sb.adds(getnm("ChooseLeft"), count);
		auto v = chooseskill(temp);
		set(v, get(v) + 1);
		count--;
	}
}