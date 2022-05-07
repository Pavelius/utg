#include "draw_object.h"
#include "main.h"

static adat<char, 32> roll_result;
player*	player::last;

static int compare(const void* p1, const void* p2) {
	return *((char*)p2) - *((char*)p1);
}

static void add_dices(int count) {
	for(auto i = 0; i < count; i++)
		roll_result.add(1 + rand() % 6);
	qsort(roll_result.data, roll_result.count, sizeof(roll_result.data[0]), compare);
}

static void roll_dices(int count) {
	roll_result.clear();
	add_dices(count);
}

static int roll_success(int sv) {
	int result = 0;
	for(auto v : roll_result) {
		if(v >= sv)
			result++;
	}
	return result;
}

int player::rolld6(int count) const {
	roll_dices(count);
	return roll_success(getsuccess());
}

void player::clear() {
	memset(this, 0, sizeof(*this));
}

int player::getsuccess() const {
	return 5;
}

static void add_clue() {
	auto p = player::last;
	p->add(Clue, -1);
	add_dices(1);
}

int player::roll(ability_s v, int m) {
	char header[128]; stringbuilder sh(header);
	sh.add("%Roll %1%+2i", getnm(bsdata<abilityi>::get(v).id), m);
	auto push_header = answers::header;
	answers::header = header;
	answers an;
	char temp[512]; stringbuilder sb(temp);
	roll_dices(get(v) + m);
	while(true) {
		auto sv = getsuccess();
		an.clear();
		sb.clear();
		if(roll_result) {
			sb.add("%Roll: ");
			for(auto i = 0; i < roll_result.getcount(); i++) {
				if(i != 0)
					sb.add(", ");
				auto v = roll_result[i];
				if(v >= sv)
					sb.add("[+%1i]", v);
				else
					sb.add("%1i", v);
			}
		} else
			sb.add(getnm("NoDicesForRoll"));
		an.add(0, getnm("ApplyRollResult"));
		if(get(Clue))
			an.add(add_clue, getnm("UseClueToAddDice"), 1);
		auto p = (fnevent)an.choose(temp);
		if(!p)
			break;
		last = this;
		p();
	}
	answers::header = push_header;
	return roll_success(getsuccess());
}

void player::encounter() {
	location->encounter();
}

static void appearobjects() {
}

void player::leavestreet() {
	if(location->type != Arkham)
		return;
	if(location->neightboard[0])
		movement(location->neightboard[0]);
}

static draw::object* getobject(const void* pv) {
	auto ps = draw::findobject(pv);
	if(ps)
		return ps;
	if(pv == static_cast<player*>(&game)) {
		auto p = (player*)pv;
		auto n = p->location;
		if(!n)
			n = bsdata<investigator>::elements[p->investigator_index].location;
		if(!n)
			return 0;
		draw::setcamera(n->position);
		ps = draw::addobject(n->position.x, n->position.y);
		ps->resource = draw::getres("characters");
		ps->frame = p->investigator_index;
		ps->data = p;
		ps->priority = 100;
	}
	return ps;
}

void player::movement(locationi* pv, bool animation) {
	auto ps = getobject(this);
	if(ps) {
		if(location && animation) {
			auto order = ps->priority;
			ps->priority = 250;
			ps->move(pv->position, 16);
			ps->priority = order;
		} else
			ps->set(pv->position);
	}
	location = pv;
}

void player::delayed() {
	auto ps = getobject(this);
	if(ps) {
		ps->alpha = 128;
		appearobjects();
	}
}

void player::losehalf(cardtype_s id) {
}

int player::getminimal(ability_s v) const {
	switch(v) {
	case Sanity: case Health: return 1;
	default: return 0;
	}
}

int player::getmaximal(ability_s v) const {
	switch(v) {
	case Sanity: return m_sanity;
	case Health: return m_health;
	default: return 0;
	}
}

void player::create(const char* id) {
	clear();
	auto p = bsdata<investigator>::find(id);
	if(!p)
		return;
	auto push_interactive = answers::interactive;
	answers::interactive = false;
	investigator_index = bsdata<investigator>::source.indexof(p);
	original.loadability(*p);
	original.abilities[Health] = p->abilities[Health];
	original.abilities[Sanity] = p->abilities[Sanity];
	for(auto v : p->extra)
		apply(v);
	update();
	abilities[Clue] += p->abilities[Clue];
	abilities[Money] += p->abilities[Money];
	abilities[Health] = getmaximal(Health);
	abilities[Sanity] = getmaximal(Sanity);
	answers::interactive = push_interactive;
	game.movement(p->location);
}

void player::apply(variant v) {
	if(v.iskind<cardprotoi>()) {
		auto& ei = bsdata<cardprotoi>::elements[v.value];
		if(bsdata<cardtypei>::elements[ei.type].cards.pick(v.value))
			addcard(v.value);
	} else if(v.iskind<cardtypei>()) {
		for(auto i = 0; i < v.counter; i++) {
			auto card = bsdata<cardtypei>::elements[v.value].cards.pick();
			if(card)
				addcard(card);
		}
	} else if(v.iskind<abilityi>())
		abilities[v.value] += v.counter;
}

void player::update() {
	loadability(original);
	m_health = original.abilities[Health];
	m_sanity = original.abilities[Sanity];
	for(auto& e : source) {
		if(!e)
			continue;
		auto& ei = e.geti();
		switch(ei.type) {
		case Ally:
			addabilities(ei);
			m_health += ei.abilities[Health];
			m_sanity += ei.abilities[Sanity];
			break;
		default:
			addabilities(ei);
			break;
		}
	}
}

const investigator&	player::geti() const {
	return bsdata<investigator>::elements[investigator_index];
}

void player::introduction() const {
	auto p = getdescription(geti().id);
	if(p) {
		auto push_header = answers::header;
		answers::header = getnm("Introduction");
		answers::message(p);
	}
}

void player::movement(int speed) {
	char temp[260]; stringbuilder sb(temp);
	while(speed > 0) {
		sb.clear();
		sb.add(getnm("WhereYouWhantToMove"), speed);
		an.clear();
		for(auto p : location->neightboard) {
			if(!p)
				break;
			an.add(p, getnm(p->id));
		}
		auto p = (locationi*)an.choose(temp, getnm("StopMoving"));
		if(!p)
			break;
		movement(p);
		answers::header = getnm(p->id);
		speed--;
	}
}

void player::setflag(gamef_s v, bool activate) {
	if(activate) {
		switch(v) {
		case Bless:
			if(is(Curse)) {
				activate = false;
				v = Curse;
			}
			break;
		case Curse:
			if(is(Bless)) {
				activate = false;
				v = Bless;
			}
			break;
		}
	}
	if(answers::interactive) {
		if(activate)
			game.information(getnm("YouGainCard"), getnm(bsdata<gamefi>::elements[v].id));
		else
			game.information(getnm("YouLoseCard"), getnm(bsdata<gamefi>::elements[v].id));
	}
	if(activate)
		flags.set(v);
	else
		flags.remove(v);
}