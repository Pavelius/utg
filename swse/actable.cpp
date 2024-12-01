#include "actable.h"
#include "answers.h"
#include "speech.h"

void stract(stringbuilder& sb, gendern gender, const char* name, const char* format, const char* format_param);

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gendern female, char separator) {
	if(!format)
		return;
	if(separator)
		sb.addsep(separator);
	stract(sb, female, name, format, format_param);
}

void actable::act(const char* format, ...) const {
	if(!answers::console)
		return;
	actv(*answers::console, format, xva_start(format), getname(), gender, ' ');
}

bool actable::actid(const char* id, const char* id_suffix) const {
	if(!answers::console)
		return false;
	char temp[64]; stringbuilder sx(temp);
	if(!id_suffix)
		id_suffix = "Act";
	sx.addv(id, 0);
	sx.addv(id_suffix, 0);
	auto pn = getnme(temp);
	if(!pn)
		return false;
	actv(*answers::console, pn, 0, getname(), gender, ' ');
	return true;
}

bool actable::iskind(variant v) const {
	return kind == v;
}

bool actable::ischaracter() const {
	return true;
}

const char* actable::getname() const {
	if(name_id != 0xFFFF)
		return speech_name(name_id);
	return kind.getname();
}