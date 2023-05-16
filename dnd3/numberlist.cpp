#include "bsreq.h"
#include "crt.h"
#include "logparse.h"
#include "numberlist.h"

typedef adat<int, 512> numbert;

using namespace log;

BSDATAC(numberlist, 512)

static const char* read_numbers(const char* p, numbert& result) {
	while(isnum(*p) || *p=='-') {
		int value;
		p = stringbuilder::read(p, value);
		p = skipwscr(p);
		result.add(value);
	}
	return p;
}

void numberlist::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[1024]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		p = readidn(p, sb);
		if(!checksym(p, '\n'))
			break;
		auto pe = bsdata<numberlist>::add();
		pe->id = szdup(temp);
		p = skipwscr(p);
		numbert source;
		p = read_numbers(p, source);
		p = skipwscr(p);
		if(source)
			pe->elements.alloc(source.getcount());
	}
	log::close();
}