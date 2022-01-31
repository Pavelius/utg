#include "main.h"
#include "widget.h"

static void print(stringbuilder& sb, variant v) {
	const char* format = "%+1i";
	switch(v.type) {
	case Special:
		switch(v.value) {
		case VisitManyTimes: case VisitRequired:
		case IfChoosedAction: case IfExistEntry:
			return;
		case ReloadGun:
		case AddGun:
		case AddGunUnloaded:
			format = 0;
			break;
		default:
			break;
		}
		break;
	default: break;
	}
	auto negative = v.counter < 0;
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
		if(p->next == 0)
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
	for(unsigned i = 0; i < 50; i++) {
		if(!game.istag(i))
			continue;
		draw::label(getentryname(i), 0);
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
	else
		utg::getstatus(object, sb);
}