#include "speech.h"
#include "main.h"

gendern npcname::getgender() const {
	return equal(speech_getid(nameid), "Female") ? Female : Male;
}

const char* npcname::getname() const {
	return speech_name(nameid);
}

void npcname::randomname() {
	nameid = speech_random(rollv(0) <= 3 ? "Female" : "Male");
	nicknameid = speech_random(rollv(0) <= 2 ? "NicknameEnd" : "Nickname");
}

void npcname::getname(stringbuilder& sb) const {
	auto first = equal(speech_getid(nicknameid), "Nickname");
	auto female = (getgender()==Female);
	if(first) {
		if(female)
			sb.addnounf(speech_name(nicknameid));
		else
			sb.adds(speech_name(nicknameid));
	}
	sb.adds(speech_name(nameid));
	if(!first)
		sb.adds(speech_name(nicknameid));
}