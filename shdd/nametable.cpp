#include "bsreq.h"
#include "nametable.h"
#include "logparse.h"

BSDATAC(nametable::element, 4096)
BSDATAC(nametable, 128)

BSMETA(nametable) = {
	BSREQ(id),
	{}};

using namespace log;

static const char* read_line(const char* p, stringbuilder& sb) {
	sb.clear();
	while(*p) {
		if(*p == '\n' || *p == '\r' || *p=='#')
			break;
		else
			sb.addch(*p++);
	}
	sb.trimr();
	return p;
}

static const char* read_script(const char* p, variants& elements, stringbuilder& sb) {
	auto pr = bsdata<variant>::source_ptr;
	if(!elements.count)
		elements.start = pr->getcount();
	while(allowparse && ischa(*p)) {
		auto pe = (variant*)pr->add();
		pe->clear();
		p = readval(p, sb, *pe);
		p = skipsp(p);
		elements.count++;
	}
	return p;
}

static const char* read_weight(const char* p, nametable::element* ps) {
	if(!isnum(*p)) {
		error(p, "Expected number");
		allowparse = false;
		return p;
	}
	int i1 = 0;
	p = stringbuilder::read(p, i1);
	if(*p == '-') {
		int i2 = 0;
		p = stringbuilder::read(skipsp(p+1), i2);
		i1 = i2 - i1 + 1;
	}
	ps->weight = i1;
	return skipsp(p);
}

static void read_content(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		auto ps = bsdata<nametable::element>::add();
		p = read_weight(p, ps);
		p = read_line(p, sb);
		if(temp[0] == 0) {
			error(p, "Expected string line");
			break;
		}
		ps->name = szdup(temp);
		if(p[0] == '#')
			p = read_script(skipsp(p + 1), ps->elements, sb);
		p = skipwscr(p);
	}
	log::close();
}

void nametable::read(const char* id) {
	char temp[260]; stringbuilder sb(temp);
	sb.addlocalefile(id, "txt");
	auto ps = bsdata<nametable>::add();
	ps->id = szdup(id);
	auto p1 = bsdata<nametable::element>::end();
	read_content(temp);
	ps->elements = slice<nametable::element>(p1, bsdata<nametable::element>::end() - p1);
}

int	nametable::getrange() const {
	auto result = 0;
	for(auto& e : elements)
		result += e.weight;
	return result;
}

const nametable::element* nametable::random() const {
	auto range = getrange();
	if(!range)
		return 0;
	return first(rand() % range);
}

const nametable::element* nametable::first(int value) const {
	auto n1 = 0;
	for(auto& e : elements) {
		auto n2 = n1 + e.weight;
		if(value >= n1 && value < n2)
			return &e;
		n1 = n2;
	}
	return 0;
}