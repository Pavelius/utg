#include "stringlist.h"
#include "main.h"

gender_s npcname::getgender() const {
	return equal(bsdata<stringlist>::elements[nameid].id, "Female") ? Female : Male;
}

const char* npcname::getname() const {
	return bsdata<stringlist>::elements[nameid].name;
}

void npcname::randomname() {
	nameid = stringlist::random(rollv(0) <= 3 ? "Female" : "Male");
	nicknameid = stringlist::random(rollv(0) <= 2 ? "NicknameEnd" : "Nickname");
}

void npcname::getname(stringbuilder& sb) const {
	auto& e1 = bsdata<stringlist>::elements[nameid];
	auto& e2 = bsdata<stringlist>::elements[nicknameid];
	auto first = equal(e2.id, "Nickname");
	auto female = (getgender()==Female);
	if(first) {
		if(female)
			sb.addnounf(e2.name);
		else
			sb.adds(e2.name);
	}
	sb.adds(e1.name);
	if(!first)
		sb.adds(e2.name);
}