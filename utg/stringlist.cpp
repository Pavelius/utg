#include "stringlist.h"
#include "logparse.h"

using namespace log;

static const unsigned max_size = 2048;
BSDATAC(stringlist, max_size)

bool stringlist::match(const char* id) const {
	return equal(this->id, id);
}

const char* stringlist::getname(unsigned short v) {
	if(v == 0xFFFF)
		return "No name";
	return bsdata<stringlist>::elements[v].name;
}

unsigned stringlist::select(short unsigned* pb, short unsigned* pe, const char* id) {
	auto ps = pb;
	for(auto& e : bsdata<stringlist>()) {
		if(!e.match(id))
			continue;
		if(ps < pe)
			*ps++ = getbse(e);
	}
	return ps - pb;
}

short unsigned stringlist::random(const char* id) {
	short unsigned temp[max_size];
	auto count = select(temp, temp + sizeof(temp) / sizeof(temp[0]), id);
	if(count)
		return temp[rand() % count];
	return 0xFFFF;
}

static bool ischax(unsigned char u) {
	return (u >= 'A' && u <= 'Z')
		|| (u >= 'a' && u <= 'z')
		|| (u >= 0xC0 && u <= 0xFF)
		|| (u >= 0x410 && u <= 0x44F);
}

static const char* read_line(const char* p, const char* id, stringbuilder& sb) {
	auto index = 0;
	while(ischax(*p)) {
		auto pe = bsdata<stringlist>::add();
		memset(pe, 0, sizeof(*pe));
		p = readname(skipws(p), sb);
		pe->id = id;
		pe->index = index;
		pe->name = getstring(sb);
		p = skipws(p);
		if(*p == 13 || *p == 10 || *p == 0)
			break;
		if(!checksym(p, ','))
			break;
		p = skipwscr(p + 1);
		index++;
	}
	return p;
}

void stringlist::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		p = readidn(p+1, sb);
		if(!checksym(p, '\n'))
			break;
		auto id = szdup(temp);
		p = read_line(skipwscr(p), id, sb);
		p = skipwscr(p);
	}
	log::close();
}

unsigned stringlist::getcount() const {
	auto pb = this;
	auto pe = bsdata<stringlist>::end();
	auto id = this->id;
	auto result = 0;
	while(pb < pe && pb->id == id)
		result++;
	return result;
}

bool apply_list(const char* identifier, stringbuilder& sb) {
	for(auto& e : bsdata<stringlist>()) {
		if(equal(e.id, identifier)) {
			auto count = e.getcount();
			sb.add((&e)[rand() % count].name);
			return true;
		}
	}
	return false;
}