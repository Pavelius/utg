#include "actable.h"
#include "answers.h"
#include "bsdata.h"
#include "groupname.h"

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

void actable::actv(const char* format, const char* format_param) const {
	if(!answers::console)
		return;
	if(answers::console->begin()[0])
		answers::console->addsep('\n');
	stract(*answers::console, gender, getname(), format, format_param);
}

void actable::act(const char* format, ...) const {
	actv(format, xva_start(format));
}

const char* actable::getname() const {
	return bsdata<groupname>::elements[name].name;
}