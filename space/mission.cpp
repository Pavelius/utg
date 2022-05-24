#include "main.h"

missioni	game;

void missioni::clear() {
	memset(this, 0, sizeof(*this));
}

void missioni::beforemove() {
}

void missioni::apply(variants source) {
	for(auto v : source)
		apply(v);
}

void missioni::apply(variant v) {
}

void actv(stringbuilder& sb, const char* format, const char* format_param, char add_sep = '\n') {
	sb.addv(format, format_param);
}