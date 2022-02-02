#include "main.h"
#include "widget.h"

static void print(stringbuilder& sb, variant v) {
	const char* format = "%+1i";
	auto negative = v.counter < 0;
	switch(v.type) {
	case Ability:
		if(bsdata<abilityi>::elements[v.value].type == Negative)
			negative = !negative;
		break;
	case Special:
		switch(v.value) {
		case VisitManyTimes: case VisitRequired:
		case IfChoosedAction: case IfEqual: case IfNonZeroForward: case IfZeroForward:
			return;
		case ReloadGun:
		case AddGun:
		case AddGunUnloaded:
			format = 0;
			break;
		default:
			if(v.value >= Page000 && v.value <= Page900)
				return;
			break;
		}
		break;
	default: break;
	}
	sb.addsep(' ');
	if(negative)
		sb.add("[-");
	sb.add(v.getname(), v.counter);
	if(v.counter && format)
		sb.add(format, v.counter);
	if(negative)
		sb.add("]");
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

void chest::listoftreasures() {
	for(auto id : *this) {
		auto p = getobject(id);
		if(!p)
			continue;
		draw::label(getnm(p->id), 0, p);
	}
}

void gamei::listoftreasures() {
	game.chest::listoftreasures();
}

void shiplog::listofrecords() {
	for(unsigned i = AnswerEntry; i < AnswerEntry + 50; i++) {
		if(!game.istag(i))
			continue;
		draw::label(getentryname(i), 0, getentry(i));
	}
}

void shiplog::listofgoals() {
	auto p = game.getgoal();
	if(p)
		draw::label(getnm(p->id), 0, p);
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