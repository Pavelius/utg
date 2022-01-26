#include "main.h"

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

void gamei::sfgetinfo(const void* object, stringbuilder& sb) {
	if(bsdata<quest>::have(object)) {
		auto p = (quest*)object;
		if(p->next==0)
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