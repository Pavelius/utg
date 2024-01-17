#include "answers.h"
#include "console.h"

static char temp[2048];
stringbuilder sbc(temp);

void initialize_console() {
	answers::prompi = temp;
}

void clear_console() {
	sbc.clear();
}

void add_console(const char* sym, const char* format, const char* format_param) {
	sbc.add("[%1", sym);
	sbc.addv(format, format_param);
	sbc.add("]");
}

void add_console(const char* format, const char* format_param) {
	sbc.addv(format, format_param);
}