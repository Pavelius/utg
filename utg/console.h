#pragma once

#include "stringbuilder.h"

extern stringbuilder sbc;

void add_console(const char* wrap, const char* format, const char* format_param);
void clear_console();
void initialize_console();