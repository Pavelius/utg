#include "calculator.h"
#include "draw.h"
#include "image.h"
#include "stringbuilder.h"

using namespace draw;

void printcnf(const char* text);

static void printv(const char* format, const char* format_param) {
	char temp[512]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printcnf(temp);
}

static void print(const char* format, ...) {
	printv(format, xva_start(format));
}

static void println(const char* format, ...) {
	printv(format, xva_start(format));
	printcnf("\r\n");
}

static void print_symbols() {
	for(auto& e : bsdata<symboli>()) {
		if(!e.ispredefined())
			println(string_name(e.ids));
	}
}

static void errorv(const char* format, const char* format_param) {
	printcnf("Error: "); printv(format, format_param);
	printcnf("\r\n");
}

static void initialize_parser() {
	calculator_error_proc = errorv;
}

int main() {
	initialize_parser();
	calculator_file_parse("code.txt");
	print_symbols();
	return 0;
}