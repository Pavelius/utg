#include "bsreq.h"
#include "log.h"
#include "numberlist.h"

typedef adat<int, 512> numbert;

using namespace log;

BSDATAC(numberlist, 512)

int	numberlist::match(int value) const {
	int count = elements.size();
	if(!count)
		return 0;
	auto p = elements.begin();
	if(value < count)
		return p[value];
	return p[count - 1];
}

static const char* read_numbers(const char* p, numbert& result) {
	while(isnum(*p) || *p == '-') {
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
			pe->elements.alloc(source.getcount(), source.data);
	}
	log::close();
}