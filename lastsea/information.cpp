#include "main.h"

static void print(stringbuilder& sb, variant v) {
	auto negative = v.counter < 0;
	if(negative)
		sb.add("[-");
	sb.adds(v.getname());
	if(v.counter)
		sb.add("%+1i", v.counter);
	if(negative)
		sb.add("]");
}

static void print(stringbuilder& sb, const variants& source) {
	for(auto v : source)
		print(sb, v);
}

void actioni::getinfo(stringbuilder& sb) const {
	print(sb, script);
}

void casei::getinfo(stringbuilder& sb) const {
	print(sb, outcome);
}

void gamei::getstatus(stringbuilder& sb, const void* object) {
	if(bsdata<actioni>::have(object))
		((actioni*)object)->getinfo(sb);
	else if(bsdata<casei>::have(object))
		((casei*)object)->getinfo(sb);
}