#include "crt.h"
#include "io_stream.h"
#include "log.h"
#include "stringbuilder.h"

bool log::allowparse = true;
static int error_count;
log::contexti log::context;

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

int getline(const char* pb, const char* pc) {
	auto p = pb;
	auto r = 0;
	while(*p && p < pc) {
		p = endline(p);
		p = skipcr(p);
		r++;
	}
	return r;
}

void log::errorv(const char* position, const char* format) {
	static io::file file("errors.txt", StreamWrite | StreamText);
	if(!file)
		return;
	error_count++;
	if(position && context.file)
		file << " Line " << getline(context.file, position) << ": ";
	file << format << "\n";
}

void log::error(const char* position, const char* format, ...) {
	char temp[4096]; stringbuilder sb(temp);
	if(context.url) {
		sb.add("In file `%1`:", context.url);
		context.url = 0;
		errorv(0, temp);
		sb.clear();
	}
	sb.addv(format, xva_start(format));
	errorv(position, temp);
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
	sb.addlocalefile(id, id, "txt");
	proc(temp);
}