#include "main.h"
#include "widget.h"

static void print(stringbuilder& sb, variant v) {
	const char* format = "%+1i";
	switch(v.type) {
	case Special:
		switch(v.value) {
		case VisitManyTimes:
			return;
		case Steal:
			format = "%1i";
			break;
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
	if(negative)
		sb.add("[-");
	sb.adds(v.getname(), v.counter);
	if(v.counter && format)
		sb.adds(format, v.counter);
	if(negative)
		sb.add("]");
}

static void print(stringbuilder& sb, const variants& source) {
	for(auto v : source)
		print(sb, v);
}

static void print(stringbuilder& sb, const char* name, int bonus, const char* format = "%1%+2i") {
	sb.adds(format, name, bonus);
}

static void printab(stringbuilder& sb, char* ability) {
	for(auto i = Exploration; i <= Navigation; i = (ability_s)(i + 1)) {
		auto v = ability[i - Exploration];
		if(!v)
			continue;
		print(sb, getnm(bsdata<abilityi>::elements[i].id), v);
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

void pirate::painttreasure() {
	for(auto v : game.pirate::treasures) {
		if(!v)
			continue;
		draw::label(v.getname(), 0, v.getpointer());
	}
}

void gamei::sfgetstatus(const void* object, stringbuilder& sb) {
	auto index = game.getindex(object);
	if(index != pathfind::Blocked)
		sb.add(getnm("CellInfo"), getnm("Ocean"), index);
	else
		utg::getstatus(object, sb);
}

void shiplog::paintgoals() {
	auto p = game.getgoal();
	if(p)
		draw::label(getnm(p->id), 0, p);
}