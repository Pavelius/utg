#include "actable.h"
#include "answers.h"
#include "stringlist.h"

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s female, char separator) {
	if(!format)
		return;
	sb.addsep(separator);
	//stract(sb, female, name, format, format_param);
}

void actable::act(const char* format, ...) const {
	if(!answers::console)
		return;
	actv(*answers::console, format, xva_start(format), getname(), gender, ' ');
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