#include "answers.h"
#include "print.h"

void printv(char separator, const char* format, const char* format_param) {
	if(!answers::console)
		return;
	if(separator)
		answers::console->addsep(separator);
	answers::console->addv(format, format_param);
}

void print(char separator, const char* format, ...) {
	printv(separator, format, xva_start(format));
}

void printn(const char* format, ...) {
	printv('\n', format, xva_start(format));
}

void prints(const char* format, ...) {
	printv(' ', format, xva_start(format));
}