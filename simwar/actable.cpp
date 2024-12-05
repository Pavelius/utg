#include "actable.h"
#include "stringbuilder.h"

static const actable* last;

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gendern gender);

static void custom_string(stringbuilder& sb, const char* id) {
	if(!apply_action(id, sb, last->getname(), last->gender))
		return;
	default_string(sb, id);
}

void actable::act(stringbuilder& sb, const char* format, ...) const {
	auto push = last; last = this;
	sb.addsep(' ');
	sb.addv(format, xva_start(format));
	last = push;
}