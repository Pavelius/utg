#include "actable.h"
#include "answers.h"
#include "charname.h"
#include "stringact.h"

static char console[1024];
stringbuilder sb(console);

const char* actable::getname() const {
	return "Курт";
}

void actable::actv(stringbuilder& sa, const char* format, const char* format_param, char separator) const {
	stringact sb(sa, getname(), gender);
	sb.addsep(separator);
	sb.addv(format, format_param);
	sa = sb;
}