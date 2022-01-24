#include "main.h"

void pirate::clearactions() {
	memset(actions, 0, sizeof(actions));
}

void pirate::clear() {
	memset(this, 0, sizeof(*this));
	historyable::clear();
	clearactions();
	for(auto& v : treasures)
		v = 0xFFFF;
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

void pirate::getpropertyst(const void* object, variant v, stringbuilder& sb) {
	auto p = (pirate*)object;
	int value;
	if(p->classid == 0xFFFF)
		return;
	switch(v.type) {
	case Ability:
		switch(v.value) {
		case Experience:
			value = p->get((ability_s)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((ability_s)v.value));
			value = p->getnextstar(value);
			if(value)
				sb.adds("(%1 %2i)", getnm("NextStar"), value);
			break;
		case Stars: case Misfortune: case Reroll:
			value = p->get((ability_s)v.value);
			if(value)
				sb.add("%1i", value);
			break;
		default:
			value = p->get((ability_s)v.value);
			sb.add("%1i", value);
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

void pirate::generate() {
	chooseclass();
}

bool pirate::match(variant v) const {
	switch(v.type) {
	case Class: return classid == v.value;
	case Gender: return getgender() == v.value;
	case Action:
		for(auto e : actions) {
			if(e == v)
				return true;
		}
		return false;
	default:
		return false;
	}
}

int	pirate::getmaximum(ability_s v) const {
	switch(v) {
	case Exploration:
	case Brawl:
	case Hunting:
	case Aim:
	case Swagger:
	case Navigation:
		return getclass().maximum[v - Exploration];
	case Experience: return 18;
	case Stars: case Level: return 5;
	default: return 10;
	}
}

void pirate::information(const char* format, ...) {
	utg::sb.addn("[+");
	actv(utg::sb, format, xva_start(format), false);
	utg::sb.add("]");
}

void pirate::warning(const char* format, ...) {
	utg::sb.addn("[-");
	actv(utg::sb, format, xva_start(format), false);
	utg::sb.add("]");
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

void pirate::checkstars() {
	auto pa = getclass().levelup;
	auto cv = get(Experience);
	for(unsigned i = 0; i < sizeof(classi::levelup) / sizeof(classi::levelup[0]); i++) {
		if(cv == pa[i]) {
			set(Stars, get(Stars) + 1);
			break;
		}
	}
}

void pirate::checkexperience(ability_s v) {
	auto pa = getclass().getearn(v);
	auto cv = get(v);
	for(unsigned i = 0; i < sizeof(classi::aim) / sizeof(classi::aim[0]); i++) {
		if(cv == pa[i]) {
			set(Experience, get(Experience) + 1);
			break;
		}
	}
}

void pirate::afterchange(ability_s v) {
	switch(v) {
	case Exploration: case Brawl: case Hunting: case Aim: case Swagger: case Navigation:
		checkexperience(v);
		break;
	case Experience:
		checkstars();
		break;
	}
}

void pirate::set(ability_s v, int i) {
	if(i < 0)
		i = 0;
	auto m = getmaximum(v);
	if(i > m)
		i = m;
	if(abilities[v] != i) {
		auto d = i - abilities[v];
		switch(v) {
		case Treasure:
			gaintreasure(i);
			break;
		default:
			if(v >= Exploration && v <= Navigation) {
				if(!confirm(v, d))
					return;
				set(Supply, get(Supply) - d);
			}
			if(v >= 0 && v <= Infamy) {
				if(d > 0)
					information("%1%+2i", getnm(bsdata<abilityi>::elements[v].id), d);
				else
					warning("%1%+2i", getnm(bsdata<abilityi>::elements[v].id), d);
				abilities[v] = i;
			}
			break;
		}
		afterchange(v);
	}
}

static void fixroll(stringbuilder& sb) {
	sb.add(getnm("YouRoll"), last_result, last_roll, last_bonus);
}

void pirate::roll() {
	char temp[260]; stringbuilder sb(temp);
	if(last_ability >= Exploration && last_ability <= Navigation) {
		last_bonus += get(last_ability);
		last_bonus += getbonus(last_ability);
	}
	while(true) {
		sb.clear();
		if(last_ability >= Exploration && last_ability <= Navigation)
			sb.add(getnm("YouRollAbility"), getnm(bsdata<abilityi>::elements[last_ability].id), last_bonus);
		else
			sb.add(getnm("RollDice"), last_bonus);
		answers an;
		an.add(0, getnm("MakeRoll"));
		auto pv = utg::choose(an, temp);
		if(!pv)
			break;
	}
	rollv(last_bonus);
	sb.clear(); fixroll(sb); information(temp);
	if(!last_action)
		return;
	auto stage = last_action->getstage(last_result);
	last_choose = 0;
	game.apply(last_action->getoutcome(stage));
	if(last_page > 0)
		adventure(last_page);
	if(last_choose > 0)
		last_action->choose(last_choose);
}

void pirate::addaction(variant v) {
	for(auto& e : actions) {
		if(e)
			continue;
		e = v;
		break;
	}
}

void pirate::gaintreasure(const treasurei* pv) {
	if(!pv)
		return;
	for(auto& e : treasures) {
		if(e)
			continue;
		e = pv;
		break;
	}
}

void pirate::gaintreasure(int count) {
	for(auto i = 0; i < count; i++) {
		auto pv = game.picktreasure();
		if(!pv)
			continue;
		information(getnm("GainTreasure"), getnm(pv->id));
		gaintreasure(pv);
	}
}

void pirate::playround() {
	char temp[260]; stringbuilder sb(temp);
	auto push_header = utg::header;
	utg::header = temp;
	for(auto v : actions) {
		last_action = v;
		if(!last_action)
			continue;
		utg::sb.clear();
		sb.clear(); sb.add("%1 - %2", push_header, getnm(last_action->id));
		game.apply(last_action->script);
		utg::pause();
	}
	utg::header = push_header;
}

int pirate::getbonus(ability_s v) const {
	auto r = 0;
	for(auto ev : treasures) {
		treasurei* p = ev;
		if(!p)
			continue;
		r += p->abilities[v - Exploration];
	}
	return r;
}

bool pirate::confirm(ability_s v, int delta) const {
	if(delta <= 0)
		return true;
	if(get(Supply) < delta)
		return false;
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm("ConfirmRaiseAbility"), getnm(bsdata<abilityi>::elements[v].id), delta);
	answers an;
	an.add((void*)1, getnm("RaiseAbilityAndLoseSuppy"), getnm(bsdata<abilityi>::elements[v].id), delta);
	an.add((void*)0, getnm("NothingToDo"));
	return utg::choose(an, temp);
}

static int compare(const void* v1, const void* v2) {
	auto e1 = *((variant*)v1);
	auto e2 = *((variant*)v2);
	auto& ei = game.getlocation();
	auto p1 = ei.getpriority(e1);
	auto p2 = ei.getpriority(e2);
	return p1 - p2;
}

void pirate::sortactions() {
	qsort(actions, sizeof(actions) / sizeof(actions[0]), sizeof(actions[0]), compare);
}

void pirate::chooseactions() {
	answers an;
	utg::sb.clear();
	clearactions();
	piratechoose san(an);
	char temp[260]; stringbuilder sb(temp);
	for(auto v : game.getlocation().actions) {
		auto p = (actioni*)v;
		if(!p)
			continue;
		sb.clear();
		sb.add("%1.", getnm(p->id));
		if(p->is(VisitManyTimes))
			sb.adds(getnm(bsdata<speciali>::elements[VisitManyTimes].id));
		an.add(p, temp);
	}
	san.choose(getnm("WhatDoYouWantToVisit"), 4);
	sortactions();
}

void pirate::adventure(int page) {
	quest::run(page, 0, utg::url, 0);
}