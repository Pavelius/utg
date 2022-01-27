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
			format = getnm("UpToLevel");
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
	sb.adds(v.getname());
	if(v.counter)
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

void gamei::getstatus(stringbuilder& sb, const void* object) {
}

void treasurei::sfgetinfo(const void* object, stringbuilder& sb) {
	auto p = (treasurei*)object;
	printab(sb, p->abilities);
}

void pirate::painttreasure() {
	auto result = 0;
	for(auto v : game.pirate::treasures) {
		if(!v)
			continue;
		draw::label(v.getname(), 0, v.getpointer());
		result++;
	}
	//if(result)
	//	widget::paint("Separator");
}