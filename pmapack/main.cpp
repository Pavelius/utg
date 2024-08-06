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
	for(auto& e : bsdata<symboli>())
		println(string_name(e.ids));
}

int main() {
	println("Size of imagei = %1i", sizeof(imagei));
	println("Size of imagea = %1i", sizeof(imagea));
	calculator_initialize();
	calculator_file_parse("code.txt");
	print_symbols();
	return 0;
}