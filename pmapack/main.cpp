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
		if(e.ispredefined() || e.type==-1)
			continue;
		auto pt = bsdata<symboli>::begin() + e.type;
		auto sid = &e - bsdata<symboli>::begin();
		println("%2 %1 : size %3i", string_name(e.ids), string_name(pt->ids), symbol_size(sid));
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
	project_compile("code/project/test");
	if(iserrors())
		return -1;
	print_symbols();
	return 0;
}