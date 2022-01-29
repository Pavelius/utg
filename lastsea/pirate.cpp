#include "main.h"

void pirate::clearactions() {
	memset(actions, 0, sizeof(actions));
}

void pirate::clear() {
	memset(this, 0, sizeof(*this));
	historyable::clear();
	clearactions();
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
	if(p->classid == 0xFFFF)
		return;
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

void pirate::generate() {
	chooseclass();
}

bool pirate::match(variant v) const {
	switch(v.type) {
	case Class: return classid == v.value;
	case Gender: return getgender() == v.value;
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
	case Infamy:
		return 6;
	case Stars: return 18;
	case Level: case Mission: case Cabine: case Threat:
		return 5;
	case Eat: return (get(Crew) + 1) / 2;
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

void pirate::afterchange(ability_s v) {
	switch(v) {
	case Exploration: case Brawl: case Hunting: case Aim: case Swagger: case Navigation:
		checkexperience(v);
		break;
	case Mission:
		captainmission();
	case Cabine:
		captaincabine();
		break;
	case Threat:
		makethreat();
		break;
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
			information(getnm("YouGainStars"));
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
		case Treasure:
			gaintreasure(i);
			break;
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

static int last_second_roll;

void pirate::addaction(indext v) {
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
	if(false) {
		char temp[260]; stringbuilder sb(temp);
		sb.add(getnm("ConfirmRaiseAbility"), getnm(bsdata<abilityi>::elements[v].id), delta);
		answers an;
		an.add((void*)1, getnm("RaiseAbilityAndLoseSuppy"), getnm(bsdata<abilityi>::elements[v].id), delta);
		an.add((void*)0, getnm("NothingToDo"));
		return utg::choose(an, temp);
	}
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

void pirate::adventure(int page) {
	quest::run(page, utg::url, utg::header);
}

ability_s pirate::chooseskill(const char* title) const {
	answers an;
	for(auto i = Exploration; i <= Navigation; i = (ability_s)(i + 1)) {
		if(get(i) >= getmaximum(i))
			continue;
		auto& ei = bsdata<abilityi>::elements[i];
		an.add(&ei, getnm(ei.id));
	}
	return (ability_s)bsdata<abilityi>::source.indexof(utg::choose(an, title));
}

static void start_adventure(int v, int* pages) {
	v = v - 1;
	if(v >= 0 && v < 5)
		game.adventure(pages[v]);
}

void pirate::captaincabine() {
	static int pages[] = {43, 44, 45, 46, 47};
	start_adventure(abilities[Cabine], pages);
}

void pirate::captainmission() {
	static int pages[] = {48, 49, 50, 51, 52};
	start_adventure(abilities[Mission], pages);
}

void pirate::makethreat() {
	static int pages[] = {791, 792, 793, 794, 795};
	start_adventure(abilities[Threat], pages);
}

static void start_page(const quest* ph) {
	utg::sb.clear();
	if(ph->header)
		utg::header = ph->header;
	if(ph->image)
		utg::url = ph->image;
	if(ph->text)
		game.act(utg::sb, ph->text);
}

static const quest* find_page(int v) {
	for(auto& e : bsdata<quest>()) {
		if(e.index == v)
			return &e;
	}
	return 0;
}

static const quest* find_next_action(const quest* p) {
	if(p && p[1].index == p[0].index && p[1].next > 100)
		return p + 1;
	return 0;
}

void pirate::chooseactions(int scene) {
	struct handler : utg::choosei {
		void apply(int index, const void* object) override {
			auto n = (quest*)object - bsdata<quest>::elements;
			pr->addaction(n);
		}
		bool isallow(int index, const void* object) const override {
			auto p = (quest*)object;
			if(game.islocked(index))
				return false;
			if(p->is(VisitManyTimes))
				return true;
			if(ismarked(index))
				return false;
			return true;
		}
		pirate* pr;
		handler(answers& an, pirate* pr) : choosei(an), pr(pr) {}
	};
	auto ph = find_page(4000 + scene);
	if(!ph)
		return;
	start_page(ph);
	answers an;
	clearactions();
	handler san(an, this);
	char temp[260]; stringbuilder sb(temp);
	for(auto p = find_next_action(ph); p; p = find_next_action(p)) {
		sb.clear();
		sb.add(p->text);
		if(p->is(VisitManyTimes))
			sb.adds(getnm(bsdata<speciali>::elements[VisitManyTimes].id));
		an.add(p, temp);
	}
	san.choose(getnm("WhatDoYouWantToVisit"), 4);
	sortactions();
}

void pirate::playactions() {
	for(auto v : actions) {
		if(!v)
			continue;
		playaction(v);
		utg::pause(getnm("NextAction"));
	}
}

static const quest* last_action;

void pirate::afterapply() {
	while(last_page || last_choose || last_scene) {
		if(last_page > 0) {
			auto v = last_page;
			last_page = 0;
			adventure(v);
			continue;
		}
		if(last_choose > 0) {
			auto v = last_choose;
			last_choose = 0;
			playchoose(v);
			continue;
		}
		if(last_scene) {
			if(game.scene != last_scene) {
				game.unlockall();
				game.scene = last_scene;
			}
			last_scene = 0;
			draw::setnext(game.playscene);
			continue;
		}
	}
}

void pirate::playaction(int id) {
	auto ph = bsdata<quest>::elements + id;
	last_action = find_page(ph->next);
	if(!last_action)
		return;
	start_page(last_action);
	last_page = last_choose = last_scene = 0;
	game.apply(last_action->tags);
	afterapply();
}

static const quest* find_stage(const quest* ph, int stage) {
	const quest* pr = 0;
	for(auto p = ph + 1; p->index == ph->index && p->next && stage >= p->next; p++)
		pr = p;
	return pr;
}

void pirate::rolldices() {
	rollv(last_bonus);
	if(get(Misfortune) > 0) {
		abilities[Misfortune]--;
		last_second_roll = 1 + rand() % 12;
		if(last_roll > last_second_roll) {
			iswap(last_second_roll, last_roll);
			last_result = last_roll + last_bonus;
		}
		information(getnm("YouRollMisfortune"), last_result, last_roll, last_bonus, last_second_roll);
	} else
		information(getnm("YouRoll"), last_result, last_roll, last_bonus, last_second_roll);
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
		auto pv = utg::choose(an, 0);
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

void pirate::makeroll() {
	char temp[260]; stringbuilder sb(temp);
	if(last_ability >= Exploration && last_ability <= Navigation) {
		last_bonus = get(last_ability);
		last_bonus += getbonus(last_ability);
	}
	answers an;
	while(true) {
		sb.clear();
		if(last_ability >= Exploration && last_ability <= Navigation)
			sb.add(getnm("YouRollAbility"), getnm(bsdata<abilityi>::elements[last_ability].id));
		sb.adds(getnm("RollDice"));
		if(get(Misfortune))
			sb.adds("%-AddMisfortune");
		if(last_bonus)
			sb.adds("%-1 %+2i", getnm("AddBonus"), last_bonus);
		sb.add(".");
		an.clear();
		an.add(0, getnm("MakeRoll"));
		auto pv = utg::choose(an, temp);
		if(!pv)
			break;
	}
}

void pirate::roll() {
	makeroll();
	confirmroll();
	if(!last_action)
		return;
	auto ps = find_stage(last_action, last_result);
	if(ps) {
		if(ps->text)
			act(utg::sb, ps->text);
		game.apply(ps->tags);
	}
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

void pirate::playchoose(int count) {
	struct handler : utg::choosei {
		void apply(int index, const void* object) override {
			auto p = (quest*)object;
			if(p)
				game.apply(p->tags);
		}
		bool isallow(int index, const void* object) const override {
			auto p = (quest*)object;
			if(p->is(VisitManyTimes))
				return true;
			if(ismarked(index))
				return false;
			return true;
		}
		pirate* pr;
		handler(answers& an, pirate* pr) : choosei(an), pr(pr) {}
	};
	if(!last_action)
		return;
	answers an; handler san(an, this);
	for(auto p = find_action_choose(last_action); p && p->index == last_action->index && p->next == 0; p++)
		an.add(p, p->text);
	san.choose(0, count);
}

void pirate::endscene(int scene) {
	auto ph = find_page(4000 + scene);
	if(ph && ph->next) {
		adventure(ph->next);
		afterapply();
	}
}

void pirate::playscene(int scene) {
	chooseactions(scene);
	playactions();
	endscene(scene);
}

const treasurei* pirate::choosetreasure(const char* title) const {
	answers an;
	for(auto v : treasures) {
		if(!v)
			continue;
		an.add(v.getpointer(), v.getname());
	}
	an.add(0, getnm("StopBury"));
	return (treasurei*)utg::choose(an, title);
}

void pirate::remove(variant v) {
	if(v.type == Card) {
		auto pb = treasures;
		auto pe = treasures + sizeof(treasures) / sizeof(treasures[0]);
		for(auto ps = pb; ps < pe; ps++) {
			if(*ps == v)
				continue;
			*pb++ = *ps;
		}
		while(pb < pe) {
			pb->clear();
			pb++;
		}
	}
}

void pirate::bury(int count) {
	char temp[260]; stringbuilder sb(temp);
	while(count--) {
		sb.clear();
		sb.add(getnm("WhatTreasureToBury"));
		if(count > 1)
			sb.adds(getnm("ChooseLeft"), count);
		auto p = choosetreasure(temp);
		if(!p)
			break;
		remove(p);
		set(Stars, get(Stars) + 1);
	}
}

void pirate::choosebonus(variant v1, variant v2) {
	answers an;
	an.add(&v1, "%Get %1%2i", getnm(bsdata<abilityi>::elements[v1.value].id), v1.counter);
	an.add(&v2, "%Get %1%2i", getnm(bsdata<abilityi>::elements[v2.value].id), v2.counter);
	auto pv = (variant*)utg::choose(an, 0);
	if(pv)
		game.apply(*pv);
}

void pirate::tradefriend() {
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