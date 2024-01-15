#include "actable.h"

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s gender) {
	stract(sb, gender, name, format, format_param);
}

void actable::act(stringbuilder& sb, const char* format, ...) const {
	sb.addsep(' ');
	actv(sb, format, xva_start(format), getname(), gender);
}