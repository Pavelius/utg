#include "actable.h"
#include "stringact.h"

static char	console_text[512];
stringbuilder console(console_text);

void actable::actv(stringbuilder& sbs, const char* format, const char* param, char add_sep) const {
	stringact sb(sbs, getname(), getgender());
	if(add_sep)
		sb.addsep(' ');
	sb.addv(format, param);
	sbs = sb;
}