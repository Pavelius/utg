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

void player::movement(locationi* pv) {
	auto ps = draw::findobject(this);
	if(ps) {
		if(location) {
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
	auto ps = draw::findobject(this);
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
	return 0;
}