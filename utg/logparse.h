#include "log.h"
#include "variant.h"

#pragma once

namespace log {
extern bool allowparse;
bool		checksym(const char* p, char sym);
const char* getstring(stringbuilder& sb);
const char* readbon(const char* p, int& bonus);
const char* readidn(const char* p, stringbuilder& result);
const char* readname(const char* p, stringbuilder& result);
const char*	readstr(const char* p, stringbuilder& result);
const char*	readval(const char* p, stringbuilder& sb, variant& result);
const char*	skipws(const char* p);
const char* skipwscr(const char* p);
}