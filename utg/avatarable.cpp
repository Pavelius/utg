#include "avatarable.h"
#include "crt.h"
#include "stringbuilder.h"

void avatarable::clear() {
	memset(this, 0, sizeof(*this));
}

void avatarable::setavatar(const char* v) {
	stringbuilder sb(avatar); sb.add(v);
}