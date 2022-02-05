#include "answers.h"
#include "recordset.h"
#include "variant.h"

void recordset::select(const array& source) {
	auto pe = source.end();
	for(auto p = source.begin(); p < pe; p += source.getsize()) {
		if(!getid(p))
			continue;
		add(p);
	}
}

void recordset::select(const array& source, fnvisible proc) {
	auto pe = source.end();
	for(auto p = source.begin(); p < pe; p += source.getsize()) {
		if(!getid(p))
			continue;
		if(!proc(p))
			continue;
		add(p);
	}
}

void* recordset::choose(const char* title, const char* cancel) const {
	if(!count)
		return 0;
	else if(count == 1)
		return data[0];
	answers an;
	for(auto v : *this)
		an.add(v, getnm(getid(v)));
	an.sort();
	return (void*)an.choose(title, cancel);
}

void* recordset::choosedef(const char* title) const {
	if(count==1)
        return data[0];
    return choose(title, 0);
}

void* recordset::choose(const char* id, const char* title, const char* cancel) {
	auto p = varianti::getsource(id);
	if(!p || !p->source)
		return 0;
	return choose(*p->source, title, cancel);
}

void* recordset::choose(const array& source, const char* title, const char* cancel) {
	recordset records;
	records.select(source);
	return records.choose(title, cancel);
}

static int compare_addr(const void* p1, const void* p2) {
	return *((int*)p1) - *((int*)p2);
}

void recordset::distinct() {
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

void recordset::shuffle() {
	zshuffle(data, count);
}

void* recordset::random() const {
	if(!count)
		return 0;
	return data[rand() % count];
}
