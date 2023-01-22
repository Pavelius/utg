#include "actable.h"
#include "stringact.h"

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s gender) {
	stringact sa(sb, name, gender);
	sa.addv(format, format_param);
	sb = sa;
}

void actable::act(stringbuilder& sb, const char* format, ...) const {
	sb.addsep(' ');
	actv(sb, format, xva_start(format), getname(), gender);
}