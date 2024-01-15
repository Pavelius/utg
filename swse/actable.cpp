#include "actable.h"
#include "answers.h"
#include "stringlist.h"

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s female, char separator) {
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

bool actable::actid(const char* id, const char* id_action) const {
	if(!answers::console)
		return false;
	char temp[260]; stringbuilder sx(temp);
	if(!id_action)
		id_action = "Action";
	sx.addv(id_action, 0);
	sx.addv(id, 0);
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
		return stringlist::getname(name_id);
	return kind.getname();
}