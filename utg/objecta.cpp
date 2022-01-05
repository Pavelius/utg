#include "answers.h"
#include "objecta.h"
#include "variant.h"

void objecta::select(const array& source) {
	auto pe = source.end();
	for(auto p = source.begin(); p < pe; p += source.getsize()) {
		if(!getid(p))
			continue;
		add(p);
	}
}

void* objecta::choose(const char* title, const char* cancel, bool interactive, const char* resid, const char* header) const {
	if(!count)
		return 0;
	answers an;
	for(auto v : *this)
		an.add(v, getnm(getid(v)));
	an.sort();
	return (void*)an.choose(title, cancel, interactive, resid, -1, header);
}

void* objecta::choosedef(const char* title) const {
	if(count==1)
        return data[0];
    return choose(title, 0, true, 0, 0);
}

void* objecta::choose(const char* id, const char* title, const char* cancel, bool interactive, const char* resid, const char* header) {
	auto p = varianti::getsource(id);
	if(!p || !p->source)
		return 0;
	return choose(*p->source, title, cancel, interactive, resid, header);
}

void* objecta::choose(const array& source, const char* title, const char* cancel, bool interactive, const char* resid, const char* header) {
	objecta objects;
	objects.select(source);
	return objects.choose(title, cancel, interactive, resid, header);
}

static int compare_addr(const void* p1, const void* p2) {
	return *((int*)p1) - *((int*)p2);
}

void objecta::distinct() {
	qsort(data, count, sizeof(data[0]), compare_addr);
	auto ps = data;
	auto pe = data + count;
	void* v0 = 0;
	for(auto p = data; p < pe; p++) {
		if(v0 == *p)
			continue;
		*ps++ = *p;
		v0 = *p;
	}
	count = ps - data;
}

void objecta::shuffle() {
	zshuffle(data, count);
}
