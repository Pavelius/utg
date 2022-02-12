#include "crt.h"
#include "logparse.h"
#include "property.h"
#include "stringbuilder.h"

BSDATAC(propertyi::value<int>, 256 * 8)
BSDATAC(propertyi::value<const char*>, 256 * 8)
BSDATAC(propertyi, 256 * 2)
BSDATAC(propertyi::typei, 16)

static propertyi::key* findv(int object, int type, array& source) {
	auto pe = source.end();
	auto sz = source.getsize();
	for(auto p = source.begin(); p < pe; p += sz) {
		if(((propertyi::key*)p)->object == object
			&& ((propertyi::key*)p)->type == type)
			return (propertyi::key*)p;
	}
	return 0;
}

static propertyi::key* addnew(array& source) {
	auto pe = source.end();
	auto sz = source.getsize();
	for(auto p = source.begin(); p < pe; p += sz) {
		if(!((propertyi::key*)p)->operator bool())
			return (propertyi::key*)p;
	}
	return (propertyi::key*)source.add();
}

static void addv(int object, int type, array& source, const void* v) {
	auto p = findv(object, type, source);
	if(!p) {
		p = addnew(source);
		p->object = object;
		p->type = type;
	}
	auto sz = source.getsize() - sizeof(propertyi::key);
	memcpy((char*)p + sizeof(propertyi::key), v, sz);
}

void propertyi::typei::add(const char* id, array* source) {
	if(!source || !id)
		return;
	auto p = (propertyi::typei*)bsdata<propertyi::typei>::source.addfind(id);
	p->source = source;
}

int propertyi::add(const char* id, int type) {
	if(!id)
		return 0;
	auto p = (propertyi*)bsdata<propertyi>::source.addfind(id);
	p->type = type;
	return getbsi(p);
}

void propertyi::initialize() {
	typei::add("Text", bsdata<propertyi::value<const char*>>::source_ptr);
	typei::add("Number", bsdata<propertyi::value<int>>::source_ptr);
	add("NoProperty", Number);
}

int getnumber(int object, int type) {
	if(!type)
		return 0;
	auto p = (propertyi::value<int>*)findv(object, type, bsdata<propertyi::value<int>>::source);
	if(p)
		return p->value;
	return 0;
}

void addnumber(int object, int type, int value) {
	setproperty(object, type, getnumber(object, type) + value);
}

void removenumber(int object, int type) {
	auto p = findv(object, type, bsdata<propertyi::value<int>>::source);
	if(p)
		p->type = 0;
}

const char* getstring(int object, int type) {
	if(!type)
		return 0;
	auto p = (propertyi::value<const char*>*)findv(object, type, bsdata<propertyi::value<const char*>>::source);
	if(p)
		return p->value;
	return 0;
}

template<typename T>
void setproperty(int object, int type, T value) {
	addv(object, type, bsdata<propertyi::value<T>>::source, &value);
}

using namespace log;

const char* propertyi::read(const char* p, int object) {
	char temp[512]; stringbuilder sb(temp);
	while(allowparse) {
		auto p1 = readidn(p, sb);
		if(!temp[0] || p1[0] != '(')
			break;
		auto pi = bsdata<propertyi>::source.findps(temp, 0, sb.getlenght());
		if(pi == -1)
			error(p, "Unknown property name `%1`", temp);
		valuei value;
		p1 = value.read(p1 + 1, sb);
		if(!checksym(p1, ')'))
			break;
		p = skipws(p1 + 1);
		if(pi == -1)
			continue;
		switch(bsdata<propertyi>::get(pi).type) {
		case Text:
			setproperty(object, pi, value.text);
			break;
		case Number:
			setproperty(object, pi, (int)value.number);
			break;
		}
	}
	return p;
}