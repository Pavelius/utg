#include "actable.h"
#include "answers.h"
#include "speech.h"

const char* actable::getname() const {
	if(name_id != 0xFFFF)
		return speech_name(name_id);
	return kind.getname();
}

void actable::act(const char* format, ...) const {
	if(!answers::console)
		return;
	answers::console->addsep(' ');
	answers::console->addv(format, xva_start(format));
}

void actable::actns(const char* format, ...) const {
	if(!answers::console)
		return;
	answers::console->addv(format, xva_start(format));
}

bool actable::actid(char separator, const char* id, const char* id_suffix, ...) const {
	if(!answers::console)
		return false;
	char temp[64]; stringbuilder sx(temp);
	sx.addv(id, 0);
	sx.addv(id_suffix, 0);
	auto pn = getnme(temp);
	if(!pn)
		return false;
	answers::console->addsep(separator);
	answers::console->addv(pn, xva_start(id_suffix));
	return true;
}

bool actable::iskind(variant v) const {
	return kind == v;
}

bool actable::ischaracter() const {
	return true;
}