#include "draw_object.h"
#include "main.h"

static adat<char, 32>	roll_result;
player*					player::last;
static ability_s		m_ability;

static int compare(const void* p1, const void* p2) {
	return *((char*)p2) - *((char*)p1);
}

static void add_dices(int count) {
	for(auto i = 0; i < count; i++)
		roll_result.add(game.d6());
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

bool player::isequiped(const cardi* e) const {
	for(auto p : hands) {
		if(p == e)
			return true;
	}
	return false;
}

void player::equipment(stringbuilder& sb) const {
	auto pb = sb.get();
	for(auto p : hands) {
		if(!p)
			continue;
		if(sb.get() != pb)
			sb.adds(getnm("And"));
		else
			sb.addn(getnm("YouWear"));
		sb.adds("%-1", getnm(p->geti().id));
	}
	if(sb.get() != pb)
		sb.add(".");
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

int player::getcluedices(ability_s v) const {
	auto r = 1;
	if(is(ExtraClueDice))
		r++;
	for(auto& e : cards) {
		if(!e || e.area != PlayerArea)
			continue;
		auto& ei = e.geti();
		if(ei.type == Skill)
			r += ei.abilities[v];
	}
	return r;
}

static void add_clue() {
	auto p = player::last;
	p->add(Clue, -1);
	add_dices(p->getcluedices(m_ability));
}

bool cardi::afterroll(ability_s v, int m, special_s special, bool run) {
	auto& ei = geti();
	auto have_failed_dices = roll_result
		&& roll_result.data[roll_result.count - 1] >= player::last->getsuccess();
	if(have_failed_dices
		&& ei.is(ExhauseToRerollDie)
		&& !exhaused) {
		if(run) {
			exhaused = 1;
			roll_result.data[roll_result.count - 1] = game.d6();
		}
	} else if(have_failed_dices
		&& ei.rerollall.is(v)) {
		if(run) {
			exhaused = 1;
			for(auto& v : roll_result) {
				if(v < player::last->getsuccess())
					v = game.d6();
			}
		}
	} else
		return false;
	return true;
}

static void use_items(ability_s v, int m, special_s special, answers& an) {
	for(auto& e : cards) {
		if(!e || e.area != PlayerArea)
			continue;
		if(e.afterroll(v, m, special, false))
			an.add(&e, getnm("UseItem"), getnm(e.geti().id));
	}
}

int player::roll(ability_s v, int m, special_s special) {
	auto push_header = answers::header;
	auto push_player = player::last;
	char header[128]; stringbuilder sh(header);
	sh.add("%Roll %1%+2i", getnm(bsdata<abilityi>::get(v).id), m);
	last = this;
	answers::header = header;
	answers an;
	char temp[512]; stringbuilder sb(temp);
	roll_dices(get(v) + m);
	m_ability = v;
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
			an.add(add_clue, getnm("UseClueToAddDice"), getcluedices(m_ability));
		use_items(v, m, special, an);
		auto result = an.choose(temp);
		if(!result)
			break;
		if(cards.indexof(result) != -1)
			((cardi*)result)->afterroll(v, m, special, true);
		else
			((fnevent)result)();
	}
	player::last = push_player;
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
			cards.add(v.value);
	} else if(v.iskind<cardtypei>()) {
		for(auto i = 0; i < v.counter; i++) {
			auto card = bsdata<cardtypei>::elements[v.value].cards.pick();
			if(card)
				cards.add(card);
		}
	} else if(v.iskind<abilityi>())
		abilities[v.value] += v.counter;
}

void player::update() {
	loadability(original);
	abilities[Speed] -= focus[0]; abilities[Sneak] += focus[0];
	abilities[Fight] -= focus[1]; abilities[Will] += focus[1];
	abilities[Lore] -= focus[2]; abilities[Luck] += focus[2];
	m_health = original.abilities[Health];
	m_sanity = original.abilities[Sanity];
	for(auto& e : cards) {
		if(!e || !e.is(PlayerArea))
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

int	player::getbonus(ability_s v, int b) const {
	auto m = getmaximal(v);
	switch(b) {
	case 100: b = game.d6();
	case 101: b = get(v) / 2;
	case 102: b = get(v);
	case -100: b = -game.d6();
	case -101: b = -get(v) / 2;;
	case -102: b = -get(v);
	}
	if(m && b > 0) {
		auto nv = get(v) + b;
		if(nv > m)
			b = m - get(v);
	}
	return b;
}

bool player::paythrophy(int count, bool run, bool gates, bool monsters) {
	answers an;
	cardquerry querry;
	if(monsters)
		querry.add(cards, Monster);
	if(gates)
		querry.add(cards, Gate);
	auto total_cost = 0;
	for(auto p : querry) {
		if(p->geti().type == Gate)
			total_cost += 5;
		else
			total_cost += p->geti().get(Toughness);
	}
	if(total_cost < count)
		return false;
	if(run) {
		char temp[128]; stringbuilder sb(temp);
		while(count > 0) {
			an.clear();
			for(auto p : querry)
				an.add(p, getnm(p->geti().id));
			sb.clear(); sb.add(getnm("PayLeft"), count);
			auto p = (cardi*)an.choose(temp);
			if(p->geti().type == Gate)
				count -= 5;
			else
				count -= p->geti().get(Toughness);
			p->discard();
			querry.remove(querry.find(p), 1);
		}
	}
	return true;
}

void player::equip(cardi* p) {
	for(auto& e : hands) {
		if(!e) {
			e = p;
			break;
		}
	}
}

void player::unequip(cardi* p) {
	for(auto& e : hands) {
		if(e == p)
			e = 0;
	}
}

void player::usehands() {
	for(auto p : hands) {
		if(p)
			p->use();
	}
}

void player::refreshcards() {
	for(auto& e : cards)
		e.exhaused = 0;
}

void player::refocus() {
	union charpvoid {
		char	c[4];
		void*	v;
	};
	auto push_header = answers::header;
	answers::header = getnm("Focus");
	char temp[260]; stringbuilder sb(temp);
	charpvoid m;
	auto count = get(Focus);
	while(count > 0) {
		an.clear();
		if(focus[0]) {
			m.v = 0; m.c[0] = 0; m.c[1] = -1;
			an.add(m.v, "%Speed+1, %Sneak-1");
		}
		if(focus[0] < 3) {
			m.v = 0; m.c[0] = 0; m.c[1] = 1;
			an.add(m.v, "%Speed-1, %Sneak+1");
		}
		if(focus[1]) {
			m.v = 0; m.c[0] = 1; m.c[1] = -1;
			an.add(m.v, "%Fight+1, %Will-1");
		}
		if(focus[1] < 3) {
			m.v = 0; m.c[0] = 1; m.c[1] = 1;
			an.add(m.v, "%Fight-1, %Will+1");
		}
		if(focus[2]) {
			m.v = 0; m.c[0] = 2; m.c[1] = -1;
			an.add(m.v, "%Lore+1, %Luck-1");
		}
		if(focus[2] < 3) {
			m.v = 0; m.c[0] = 2; m.c[1] = 1;
			an.add(m.v, "%Lore-1, %Luck+1");
		}
		sb.clear(); sb.add(getnm("ChangeFocus"), count);
		m.v = an.choose(temp, getnm("DoNotChangeStats"));
		if(!m.v)
			break;
		focus[m.c[0]] += m.c[1];
		update();
		count--;
	}
	answers::header = push_header;
}

void player::showcards() {
	an.clear();
	for(auto& e : cards) {
		if(!e && e.area != PlayerArea)
			continue;
		an.add(&e, getnm(e.geti().id));
	}
	an.choose(0, getnm("Cancel"), 1);
}

int player::getcount(cardtype_s v) const {
	int r = 0;
	for(auto& e : cards) {
		if(!e && e.area != PlayerArea)
			continue;
		if(e.geti().type == v)
			r++;
	}
	return r;
}
