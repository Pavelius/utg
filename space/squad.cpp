#include "stringact.h"
#include "main.h"

BSDATAC(squadi, 128)

void squad::actv(stringbuilder& sbs, const char* format, const char* format_param) {
	stringact sb(sbs, getname(), getgender());
	sb.addsep(' ');
	sb.addv(format, format_param);
	sbs = sb;
}

void squad::clear() {
	memset(this, 0, sizeof(*this));
	type = 0xFF;
}

void squad::getinfo(stringbuilder& sb) const {
}