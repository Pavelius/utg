#include "nameable.h"
#include "stringbuilder.h"

const char*	nameable::getname() const {
	return getnm(id);
}

const char*	nameable::getinfo() const {
	return getnme(ids(id, "Info"));
}