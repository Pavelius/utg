#include "stringact.h"
#include "npc.h"

static char	console_text[512];
stringbuilder console(console_text);

static void add_identifier(stringbuilder& sb, const char* id) {
	act_identifier(sb, id);
}

void initialize_str() {
	stringbuilder::custom = add_identifier;
}

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, char add_sep) const {
	if(!format)
		return;
	if(add_sep)
		sb.addsep(' ');
	stract(sb, getgender(), getname(), format, format_param);
}