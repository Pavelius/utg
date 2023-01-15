#include "army.h"
#include "costitem.h"
#include "player.h"

void add_line(stringbuilder& sb, const costac& source);
int get_value(const char* id, int value);

static int compare_units(const void* v1, const void* v2) {
	auto p1 = *((uniti**)v1);
	auto p2 = *((uniti**)v2);
	if(p1->effect[Level] != p2->effect[Level])
		return p1->effect[Level] - p2->effect[Level];
	return p2->effect[Health] - p1->effect[Health];
}

struct stringarmy : stringact {
	const army* pa;
	stringarmy(stringbuilder& sb, const army* pa) : stringact(sb, pa->getname(), NoGender), pa(pa) {}
	void addidentifier(const char* identifier) {
		if(equal(identifier, "Name"))
			add(pa->getname());
		else if(equal(identifier, "NameOf")) {
			add(pa->getnameof());
			if(pa->hero)
				adds(getnm("LeadedBy"), pa->hero->getname());
		} else if(equal(identifier, "NameOfNoHero"))
			add(pa->getnameof());
		else if(equal(identifier, "Units"))
			pa->addunits(*this);
		else if(equal(identifier, "UnitsAll"))
			pa->addunits(*this, false);
		else if(equal(identifier, "Spoils"))
			add_line(*this, pa->spoils);
		else if(szstart(identifier, "Total")) {
			auto pv = bsdata<costi>::find(identifier + 5);
			if(pv) {
				auto v = (cost_s)(pv - bsdata<costi>::elements);
				pa->addtotal(*this, v);
			}
		} else
			stringact::addidentifier(identifier);
	}
};

void army::addunits(stringbuilder& sb, bool use_distinct) const {
	collection<uniti> source;
	for(auto p : *this)
		source.add(const_cast<uniti*>(p));
	if(use_distinct)
		source.distinct();
	source.sort(compare_units);
	int count = source.getcount();
	for(int i = 0; i < count; i++) {
		if(i > 0) {
			if(i == count - 1)
				sb.adds("%-1", getnm("And"));
			else
				sb.add(getnm(","));
		}
		sb.adds(source[i]->getname());
	}
}

void army::act(stringbuilder& sb, const char* format, ...) const {
	stringarmy sa(sb, this);
	sa.addv(format, xva_start(format));
	sb = sa;
}

const char*	army::getname() const {
	if(player)
		return player->getname();
	return getnm("IndependedForce");
}

const char*	army::getnameof() const {
	if(player)
		return getnm(str("%1Of", player->id));
	return getnm("IndependedForceOf");
}

void army::addprompt(stringbuilder& sb) const {
	sb.adds("%-1", getnameof());
	if(hero)
		sb.adds(getnm("LeadedBy"), hero->getname());
}

void army::addtotal(stringbuilder& sb, cost_s v) const {
	auto push_cost = lastcostitem; costitema it;
	lastcostitem = &it;
	auto total = get(v);
	char temp[512]; stringbuilder sba(temp);
	add_cost_items(sba);
	lastcostitem = push_cost;
	sb.adds("[({"); sb.addv(temp, 0); sb.add("})%1i]", total);
}

int army::geteffect(cost_s v) const {
	auto result = 0;
	for(auto p : *this)
		result += get_value(p->id, p->effect[v]);
	return result;
}

int army::get(cost_s v) const {
	int result = geteffect(v);
	if(hero)
		result += get_value(hero->id, hero->effect[v]);
	if(tactic) {
		result += get_value(tactic->id, tactic->effect[v]);
		result += get_value(tactic->id, tactic->perunit[v] * getcount());
	}
	if(province)
		result += get_value(province->id, province->current[v]);
	return result;
}

int army::get(cost_s v, const army* opponent, cost_s mv) const {
	int result = get(v);
	auto push_cost = lastcostitem;
	lastcostitem = 0; auto value = opponent->get(mv);
	lastcostitem = push_cost;
	if(value > result)
		value = result;
	result += get_value(bsdata<costi>::elements[mv].id, -value);
	return result;
}

void army::select(const provincei* province) {
	for(auto& e : bsdata<troop>()) {
		if(e && e.province == province)
			add(e.type);
	}
}

void army::select(const provincei* province, const playeri* player) {
	for(auto& e : bsdata<troop>()) {
		if(e && e.moveto == province && e.player == player)
			add(e.type);
	}
}

void army::fill(tactica& source) {
	for(auto i = 0; i < 3; i++)
		source.add(bsdata<tactici>::elements + i);
	for(auto p : *this)
		source.add(p->tactics);
	if(hero)
		source.add(hero->tactics);
}

void army::randomtactic() {
	tactica source; fill(source);
	tactic = source.random();
}

int	army::getunitcount(const tactici* v) const {
	auto result = 0;
	auto vi = getbsi(v);
	for(auto p : *this) {
		if(p->tactics.is(vi))
			result++;
	}
	return result;
}

void army::match(cost_s v, bool keep) {
	auto ps = begin();
	auto pe = end();
	for(auto p = ps; p < pe; p++) {
		bool valid = (*p)->effect[v];
		if(valid != keep)
			continue;
		*ps++ = *p;
	}
	count = ps - data;
}

void army::sort() {
	qsort(data, count, sizeof(data[0]), compare_units);
}

void army::normalize() {
	auto ps = data;
	auto pe = end();
	for(auto p = ps; p < pe; p++) {
		if(!(*p))
			continue;
		*ps++ = *p;
	}
	count = ps - data;
}

void army::damage(army& result, int value) {
	if(value <= 0)
		return;
	else if(value > armor) {
		value -= armor;
		armor = 0;
	} else {
		armor -= value;
		value = 0;
	}
	if(value <= 0)
		return;
	for(unsigned i = 0; i < count; i++) {
		auto hits = data[i]->effect[Health];
		if(hits > value) {
			hits -= value;
			value = 0;
		} else {
			result.add(data[i]);
			value -= hits;
			data[i] = 0;
		}
		if(!value)
			break;
	}
	normalize();
}

void army::casualty(const army& list, army& enemy) {
	for(auto p : list) {
		auto pu = isattacker() ? find_troop(p, province, player) : find_troop(p, province);
		if(pu) {
			enemy.spoils[Gold] += pu->type->effect[Strenght] * 2;
			enemy.spoils[Fame] += pu->type->effect[Strenght];
			pu->clear();
		}
	}
}

void army::setcasualty(const army& source) {
	*this = source;
	count = 0; memset(data, 0, sizeof(data));
}