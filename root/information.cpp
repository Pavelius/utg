#include "main.h"

static void addprompt(stringbuilder& sb, const char* start, const char* prompt) {
	if(!start)
		return;
	if(start[0]==0) {
		if(prompt)
			sb.adds(prompt);
	} else
		sb.add(",");
}

static void addn(stringbuilder& sb, const char* id, int v, const char* start, const char* prompt) {
	if(!v)
		return;
	addprompt(sb, start, prompt);
	sb.adds("%1 %2i", getnm(id), v);
}

harm_s harmable::getdefault() const {
	for(auto& v : harm) {
		if(v)
			return harm_s(&v - harm);
	}
	return harm_s(0);
}

void harmable::getinfo(stringbuilder& sb, const char* prompt) const {
	auto start = sb.get(); start[0] = 0;
	for(auto i = Injury; i <= Value; i = harm_s(i+1)) {
		auto v = harm[i];
		if(!v)
			continue;
		addprompt(sb, start, prompt);
		sb.adds("[%2i] %1", getnm(bsdata<harmi>::elements[i].id), v);
	}
	if(start[0])
		sb.add(".");
}