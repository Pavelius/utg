#include "code_parser.h"
#include "io_stream.h"
#include "log.h"
#include "pushvalue.h"
#include "stringbuilder.h"
#include "viewpackage.h"

unsigned long getcputime();

using namespace code;

const char* code::project;
const char* code::library;

static viewpackage* find_exist(const char* id) {
	for(auto& e : bsdata<viewpackage>()) {
		auto pn = e.getstr(0);
		if(equal(pn, id))
			return &e;
	}
	return 0;
}

static bool exist_url(stringbuilder& sb, const char* folder, const char* id, const char* ext) {
	sb.clear();
	if(folder) {
		sb.add(folder);
		sb.add("/");
	}
	sb.add(id);
	sb.add(".");
	if(ext)
		sb.add(ext);
	return io::file::exist(sb.begin());
}

static bool geturl(stringbuilder& sb, const char* id, const char* ext) {
	char temp[260]; stringbuilder sx(temp);
	sx.add(id); sx.change('.', '/');
	if(exist_url(sb, project, id, ext))
		return true;
	if(exist_url(sb, library, id, ext))
		return true;
	return false;
}

void viewpackage::clear() {
	package::clear();
	stamp = 0;
	flags = 0;
}

viewpackage* code::openview(const char* id) {
	auto p = find_exist(id);
	if(!p) {
		char temp[260]; stringbuilder sb(temp);
		p = bsdata<viewpackage>::add();
		p->clear();
		if(geturl(sb, id, "c2")) {
			p->source = loadt(temp);
			p->create(id);
			//log::setfile(p->source);
			//pushvalue push(last_package, static_cast<package*>(p));
			//code::parse(p->source, 0);
			//log::setfile(0);
		}
	}
	return p;
}