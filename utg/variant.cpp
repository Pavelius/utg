#include "variant.h"

static const char* match(const char* text, const char* name) {
	while(*name) {
		if(*name++ != *text++)
			return 0;
	}
	return text;
}

template<> variant::variant(const void* v) : u(0) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source)
			continue;
		auto i = e.source->indexof(v);
		if(i != -1) {
			value = i;
			type = &e - bsdata<varianti>::elements;
			break;
		}
	}
}

template<> variant::variant(const char* v) : u(0) {
	if(v) {
		auto size = zlen(v);
		for(auto& e : bsdata<varianti>()) {
			if(!e.source || !e.metadata || e.key_count != 1)
				continue;
			int i = e.source->findps(v, 0, size);
			if(i != -1) {
				value = i;
				type = &e - bsdata<varianti>::elements;
				counter = 0;
				break;
			}
		}
	}
}

const char* variant::getname() const {
	auto& e = geti();
	if(!e.source)
		return getnm("NoVariant");
	return e.getname(getpointer());
}

const char* variant::getid() const {
	auto& e = geti();
	if(!e.source)
		return "NoVariant";
	return e.getid(getpointer());
}

int varianti::found(const char* id, size_t size) const {
	int i = -1;
	if(isnamed())
		i = source->findps(id, 0, size);
	return i;
}

const char* varianti::getname(const void* object) const {
	if(pgetname)
		return pgetname(object);
	if(isnamed()) {
		auto id = *((const char**)object);
		if(id)
			return getnm(id);
	}
	return getnm("NoName");
}

const char* varianti::getid(const void* object) const {
	if(isnamed())
		return *((const char**)object);
	return "NoName";
}

const varianti* varianti::find(const void* object) {
	if(object) {
		for(auto& e : bsdata<varianti>()) {
			if(e.source && e.source->have(object))
				return &e;
		}
	}
	return 0;
}