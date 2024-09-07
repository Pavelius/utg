#include "archive.h"
#include "bsreq.h"
#include "collection.h"
#include "log.h"
#include "stringbuilder.h"
#include "variant.h"

using namespace log;

static void serial_object(archive& f, void* object, const bsreq* type);
static void serial_requisit(archive& f, void* object, const bsreq& type);

static void serial_string(archive& f, void* object) {
	char temp[4096];
	unsigned size = 0;
	if(f.writemode) {
		auto p = *((const char**)object);
		if(p) {
			size = zlen(p);
			f.set(size);
			if(size)
				f.set((void*)p, size);
		} else
			f.set(size);
	} else {
		auto p = temp;
		f.set(size);
		if(!size)
			*((const char**)object) = 0;
		else {
			if(size > (sizeof(temp) / sizeof(temp[0]) - 1))
				p = new char[size + 1];
			f.set(p, size);
			p[size] = 0;
			*((const char**)object) = szdup(p);
			if(p != temp)
				delete[] p;
		}
	}
}

static varianti* find_type(const bsreq* type) {
	for(auto& e : bsdata<varianti>()) {
		if(e.metadata == type)
			return &e;
	}
	return 0;
}

static void serial_reference(archive& f, void* object, const bsreq* type) {
	if(!type->is(KindText))
		return;
	auto pv = *((void**)object);
	if(f.writemode) {
		if(!pv)
			f.set(pv);
		else
			serial_string(f, pv);
	} else {
		const char* id = 0;
		serial_string(f, &id);
		*((void**)object) = 0;
		if(id) {
			auto ps = find_type(type);
			if(ps) {
				auto p = ps->source->findv(id, 0);
				if(!p) {
					p = ps->source->add();
					*((const char**)p) = id;
				}
				*((void**)object) = p;
			}
		}
	}
}

static void serial_requisit(archive& f, void* object, const bsreq& type) {
	if(type.is(KindNumber) || type.is(KindEnum) || type.is(KindFlags) || type.is(KindDSet))
		f.set(object, type.lenght);
	else if(type.is(KindADat)) {
		if(!type.type)
			return; // Error: type unknown
		auto p = (adat<int, 128>*)object;
		f.set(p->count);
		for(size_t i = 0; i < p->count; i++)
			serial_object(f, (char*)p->data + type.size * i, type.type);
	} else if(type.is(KindScalar)) {
		if(!type.type)
			return; // Error: type unknown
		serial_object(f, object, type.type);
	} else if(type.is(KindText))
		serial_string(f, object);
	else if(type.is(KindReference))
		serial_reference(f, object, type.type);
	else if(type.is(KindSlice))
		return; // Not support
}

static void serial_object(archive& f, void* object, const bsreq* type) {
	for(auto p = type; *p; p++) {
		if(p->is(KindNumber) || p->is(KindEnum) || p->is(KindFlags) || p->is(KindDSet))
			f.set(p->ptr(object), p->lenght);
		else {
			for(unsigned i = 0; i < p->count; i++)
				serial_requisit(f, p->ptr(object, i), *p);
		}
	}
}

void write_object(archive& f, const collectiona& source, const bsreq* type) {
	for(auto& e : source)
		serial_object(f, e, type);
}

void serial_object(archive& f, array& source, const bsreq* type) {
	if(!type)
		return;
	if(f.writemode) {
		f.set(source.count);
	} else {
		size_t size;
		f.set(size);
		source.reserve(size);
		source.setcount(size);
	}
	auto pe = source.end();
	for(auto p = source.begin(); p < pe; p += source.element_size)
		serial_object(f, (void*)p, type);
}