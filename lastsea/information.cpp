#include "main.h"
#include "widget.h"

static void print(stringbuilder& sb, ability_s v, int count) {
	auto& ei = bsdata<abilityi>::elements[v];
	auto negative = count < 0;
	if(ei.is(Negative))
		negative = !negative;
	sb.addsep(' ');
	if(negative)
		sb.add("[-");
	sb.add("%1%+2i", getnm(ei.id), count);
	if(negative)
		sb.add("]");
}

static void print(stringbuilder& sb, special_s v, int count) {
	auto& ei = bsdata<speciali>::elements[v];
	if(v == VisitManyTimes || v == VisitRequired)
		return;
	if(v >= Page000 && v <= Page900)
		return;
	if(v >= Tile000 && v <= Tile900)
		return;
	if(v >= IfEqual)
		return;
	sb.addsep(' ');
	const char* name = getnm(ei.id);
	switch(v) {
	case EatSupply:
		sb.add("[-%Supply%+1i]", -game.getmaximum(Eat));
		break;
	default:
		sb.add(name, count);
		break;
	}
}

static void print(stringbuilder& sb, variant v) {
	switch(v.type) {
	case Ability: print(sb, (ability_s)v.value, v.counter); break;
	case Special: print(sb, (special_s)v.value, v.counter); break;
	default: break;
	}
}

static void print(stringbuilder& sb, const variants& source) {
	for(auto v : source)
		print(sb, v);
}

static void print(stringbuilder& sb, const char* format) {
	sb.addv(format, 0);
}

static void printab(stringbuilder& sb, char* ability) {
	for(auto i = Exploration; i <= Navigation; i = (ability_s)(i + 1)) {
		auto v = ability[i - Exploration];
		if(v)
			print(sb, variant(Ability, i, v));
	}
}

void gamei::information(const char* format, ...) {
	utg::sb.addn("[+");
	utg::sb.addv(format, xva_start(format));
	utg::sb.add("]");
}

void gamei::warning(const char* format, ...) {
	utg::sb.addn("[-");
	utg::sb.addv(format, xva_start(format));
	utg::sb.add("]");
}

void gamei::information(ability_s v, int count) {
	auto& ei = bsdata<abilityi>::elements[v];
	auto negative = count < 0;
	if(ei.is(Negative))
		negative = !negative;
	if(negative)
		warning("%1%+2i", getnm(ei.id), count);
	else
		information("%1%+2i", getnm(ei.id), count);
}

void gamei::sfgetinfo(const void* object, stringbuilder& sb) {
	if(bsdata<quest>::have(object)) {
		auto p = (quest*)object;
		if(p->index>=AnswerEntry && p->index < AnswerEntry+100)
			print(sb, p->text);
		else if(p->next == 0)
			print(sb, p->tags);
		else {
			auto ph = quest::find(p->next);
			if(ph)
				print(sb, p->tags);
		}
	}
}

void treasurei::sfgetinfo(const void* object, stringbuilder& sb) {
	auto p = (treasurei*)object;
	printab(sb, p->abilities);
}

void gamei::sfgetstatus(const void* object, stringbuilder& sb) {
	auto index = game.getindex(object);
	if(index != pathfind::Blocked)
		sb.add(getnm("CellInfo"), getnm("Ocean"), index);
	else if(bsdata<quest>::have(object))
		sfgetinfo(object, sb);
	else
		utg::getstatus(object, sb);
}

static void listoftreasures() {
	for(auto p : game.gettreasures())
		draw::label(getnm(p->id), 0, p);
}

static void listofrecords() {
	for(unsigned i = AnswerEntry; i < AnswerEntry + 50; i++) {
		if(!game.istag(i))
			continue;
		draw::label(game.getentryname(i), 0, game.getentry(i));
	}
}

static void listofgoals() {
	auto p = game.getgoal();
	if(p)
		draw::label(getnm(p->id), 0, p);
}

static void listofcounters() {
	char temp[64]; stringbuilder sb(temp);
	for(auto i = 0; i < variables.getcount(); i++) {
		auto pn = variables.getname(i);
		if(!pn)
			continue;
		auto v = variables.get(i);
		sb.clear(); sb.add("%1i", v);
		draw::label(pn, temp, 0);
	}
}

void initialize_information_widgets() {
	widget::add("ListOfGoals", listofgoals);
	widget::add("ListOfRecords", listofrecords);
	widget::add("ListOfTreasures", listoftreasures);
	widget::add("ListOfCounters", listofcounters);
}