#include "crt.h"
#include "io_stream.h"
#include "log.h"
#include "stringbuilder.h"

log::contexti log::context;

bool log::allowparse = true;
int log::error_count;

static void print_file_error(const char* format) {
	static io::file file("errors.txt", StreamWrite | StreamText);
	if(!file)
		return;
	file << format;
}
log::fnprint log::print_proc = print_file_error;

void log::printv(const char* format) {
	if(!print_proc)
		return;
	print_proc(format);
}

void log::printv(const char* format, const char* format_param) {
	char temp[4096]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printv(temp);
}

void log::print(const char* format, ...) {
	printv(format, xva_start(format));
}

void log::println() {
	printv("\n\r");
}

void log::errorv(const char* position, const char* format, const char* format_param) {
	error_count++;
	if(context.url) {
		print("In file `%1`:", context.url);
		println();
		context.url = 0;
	}
	if(position && context.file)
		print(" Line %1i: ", getline(context.file, position));
	printv(format, format_param);
	println();
}

void log::error(const char* position, const char* format, ...) {
	errorv(position, format, xva_start(format));
}

void log::setfile(const char* v) {
	context.file = v;
}

void log::seturl(const char* v) {
	context.url = v;
}

const char* log::read(const char* url, bool error_if_not_exist) {
	auto p_alloc = loadt(url);
	if(!p_alloc) {
		context.url = 0;
		if(error_if_not_exist)
			error(0, "Can't find file '%1'", url);
		return 0;
	}
	seturl(url);
	setfile(p_alloc);
	return p_alloc;
}

void log::close() {
	if(context.file)
		delete context.file;
	context.file = 0;
}

const char* endline(const char* p) {
	while(*p && !(*p == 10 || *p == 13))
		p++;
	return p;
}

int log::getline(const char* pb, const char* pc) {
	auto p = pb;
	auto r = 0;
	while(*p && p < pc) {
		p = endline(p);
		p = skipcr(p);
		r++;
	}
	return r;
}

int log::geterrors() {
	return error_count;
}

const char* log::skipws(const char* p) {
	while(true) {
		if(*p == ' ' || *p == 9) {
			p++;
			continue;
		}
		if(p[0] == '\\' && (p[1] == 10 || p[1] == 13)) { // Trail symbol
			p++;
			while(*p == 10 || *p == 13)
				p++;
			continue;
		}
		if(p[0] == '/' && p[1] == '/') { // End line comment
			p += 2;
			while(*p && *p != 10 && *p != 13)
				p++;
		}
		if(p[0] == '/' && p[1] == '*') { // Complex comment
			p += 2;
			while(*p) {
				if(p[0] == '*' && p[1] == '/') {
					p += 2;
					break;
				}
				p++;
			}
			continue;
		}
		break;
	}
	return p;
}

const char* log::skipwscr(const char* p) {
	while(true) {
		p = skipws(p);
		if(*p == 10 || *p == 13) {
			while(*p == 10 || *p == 13)
				p++;
			continue;
		}
		break;
	}
	return p;
}

void log::readloc(fnread proc, const char* folder, const char* id) {
	char temp[260]; stringbuilder sb(temp);
	sb.addlocalefile(folder, id, "txt");
	proc(temp);
}

void log::readlocfolder(fnread proc, const char* folder, const char* filter) {
	char temp2[260]; stringbuilder s2(temp2);
	char temp[260]; stringbuilder sb(temp);
	sb.addlocaleurl(folder);
	for(io::file::find find(temp); find; find.next()) {
		auto pn = find.name();
		if(!pn || pn[0] == '.')
			continue;
		if(filter) {
			if(!szpmatch(pn, filter))
				continue;
		}
		s2.clear();
		s2.add(temp);
		s2.add(pn);
		proc(temp2);
	}
}