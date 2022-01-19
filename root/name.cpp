#include "main.h"
#include "stringact.h"

void nameable::actv(stringbuilder& sbs, const char* format, const char* param) const {
	stringact sb(sbs, getname(), getgender());
	sb.addsep(' ');
	sb.addv(format, param);
	sbs = sb;
}