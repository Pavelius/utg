#include "charname.h"
#include "main.h"

gender_s npcname::getgender() const {
	return (gender_s)bsdata<charname>::elements[nameid].conditions[0].value;
}

const char* npcname::getname() const {
	return bsdata<charname>::elements[nameid].name;
}

void npcname::randomname() {
	variant conditions[1];
	if(rollv(0) <= 3)
		conditions[0] = Female;
	else
		conditions[0] = Male;
	nameid = charname::random(conditions);
	if(rollv(0) <= 2)
		conditions[0] = NicknameEnd;
	else
		conditions[0] = Nickname;
	nicknameid = charname::random(conditions);
}

void npcname::getname(stringbuilder& sb) const {
	auto& e1 = bsdata<charname>::elements[nameid];
	auto& e2 = bsdata<charname>::elements[nicknameid];
	auto first = (e2.conditions[0].type == Special && e2.conditions[0].value == Nickname);
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