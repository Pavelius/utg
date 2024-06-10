#include "actable.h"
#include "stringbuilder.h"

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

void actable::act(stringbuilder& sb, const char* format, ...) const {
	sb.addsep(' ');
	stract(sb, gender, format, xva_start(format), getname());
}