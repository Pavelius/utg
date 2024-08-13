#include "calculator.h"
#include "draw.h"
#include "image.h"
#include "log.h"
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

static void symbol_info(stringbuilder& sb, const symboli& e) {
	auto sid = e.getindex();
	if(e.type != -1) {
		auto& et = bsdata<symboli>::get(e.type);
		sb.add(string_name(et.ids));
		sb.add(" ");
	}
	sb.add(string_name(e.ids));
	if(e.is(Function)) {
		sb.add("(");
		auto param_count = 0;
		for(auto& m : bsdata<symboli>()) {
			if(m.parent != sid)
				continue;
			if(param_count > 0)
				sb.add(", ");
			symbol_info(sb, m);
			param_count++;
		}
		sb.add(")");
	}
}

static void print_types() {
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : bsdata<symboli>()) {
		if(e.ispredefined() || e.type==-1 || e.ispointer() || !e.istype())
			continue;
		auto pt = bsdata<symboli>::begin() + e.type;
		auto sid = &e - bsdata<symboli>::begin();
		println("Module %1 (size %2i)", string_name(e.ids), e.instance.size);
		for(auto& m : bsdata<symboli>()) {
			if(m.ispredefined() || m.type == -1 || m.parent!=sid)
				continue;
			sb.clear(); symbol_info(sb, m);
			println(sb);
		}
	}
}

static void print_symbols() {
	for(auto& e : bsdata<symboli>()) {
		if(e.ispredefined() || e.type==-1)
			continue;
		auto pt = bsdata<symboli>::begin() + e.type;
		auto sid = e.getindex();
		print("%2 %1", string_name(e.ids), string_name(pt->ids));
		auto size = e.instance.size;
		if(size)
			print(" size(%1i)", size);
		auto frame = e.instance.frame;
		if(frame)
			print(" frame(%1i)", frame);
		println();
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

static void parse_test() {
	initialize_parser();
	project_compile("code/project/test");
	if(iserrors())
		return;
	print_types();
}

static void print_error(const char* position, const char* format) {
	log::error_count++;
	if(position && log::context.file)
		print(" Line %1i:", log::getline(log::context.file, position));
	print(format);
	println();
}

static void image_test() {
	auto push_error = log::error_proc;
	log::error_proc = print_error;
	image_read("example.txt");
	log::error_proc = push_error;
	if(log::geterrors() > 0)
		return;
	println("Read %1i header and %2i elements", bsdata<imagea>::source.getcount(), bsdata<imagei>::source.getcount());
}

int main() {
	//parse_test();
	image_test();
	return 0;
}