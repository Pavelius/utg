#include "main.h"
#include "stringact.h"

void nameable::actv(stringbuilder& sbs, const char* format, const char* param, char add_sep) const {
	stringact sb(sbs, getname(), getgender());
	if(add_sep)
		sb.addsep(' ');
	sb.addv(format, param);
	sbs = sb;
}