#include "actable.h"
#include "answers.h"
#include "groupname.h"
#include "stringact.h"

static char console[1024];
stringbuilder sb(console);

const char* actable::getname() const {
	return (name == 0xFFFF) ? 0 : bsdata<groupname>::elements[name].name;
}

void actable::actv(stringbuilder& sa, const char* format, const char* format_param, char separator, const char* name, gender_s gender) {
	stringact sb(sa, name, gender);
	sb.addsep(separator);
	sb.addv(format, format_param);
	sa = sb;
}