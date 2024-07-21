#include "answers.h"
#include "console.h"

static char console_text[2048];
stringbuilder sbc(console_text);

void initialize_console() {
	answers::prompi = console_text;
}

void clear_console() {
	sbc.clear();
}

void add_console(const char* sym, const char* format, const char* format_param) {
	if(!format || format[0] == 0)
		return;
	sbc.addsep('\n');
	sbc.add("[%1", sym);
	sbc.addv(format, format_param);
	sbc.add("]");
}

void add_console(char symbol, const char* format, const char* format_param) {
	if(!format || format[0] == 0)
		return;
	sbc.addsep(symbol);
	sbc.addv(format, format_param);
}

void add_console(const char* format, const char* format_param) {
	sbc.addv(format, format_param);
}

void act(const char* format, ...) {
	if(!answers::console)
		return;
	answers::console->addsep(' ');
	answers::console->addv(format, xva_start(format));
}

void actn(const char* format, ...) {
	if(!answers::console)
		return;
	answers::console->addsep('\n');
	answers::console->addv(format, xva_start(format));
}

void information(const char* format, ...) {
	add_console("+", format, xva_start(format));
}

void warning(const char* format, ...) {
	add_console("-", format, xva_start(format));
}

void output(const char* format, ...) {
	if(!format || format[0] == 0)
		return;
	sbc.addsep('\n');
	sbc.addv(format, xva_start(format));
}