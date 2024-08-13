#include "calculator.h"
#include "draw.h"
#include "image.h"
#include "stringbuilder.h"

using namespace draw;

static const char* last_url_error;

void printcnf(const char* text);

static void println() {
	printcnf("\r\n");
}

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
	println();
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

static void errorv(const char* url, const char* format, const char* format_param, const char* example) {
	if(last_url_error != url) {
		printcnf("Error when parsing `"); printcnf(url); printcnf("`");
		println();
		last_url_error = url;
	}
	printcnf(" "); printv(format, format_param);
	if(example) {
		printcnf(" in `"); printcnf(example); printcnf("`");
	}
	println();
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