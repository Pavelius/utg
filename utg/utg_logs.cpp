#include "objecta.h"
#include "utg.h"

const char* logs::url;
const char* logs::header;

const char* logs::getchooseprompt(const char* id) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("Choose%1", id);
	return getnm(temp);
}

void* logs::choose(const char* id, const char* cancel) {
	return objecta::choose(id, getchooseprompt(id), cancel, true, url, header);
}

int logs::choosei(const char* id, const char* cancel) {
	auto p = varianti::getsource(id);
	if(!p)
		return -1;
	return p->source->indexof(objecta::choose(*p->source, getchooseprompt(id), cancel, true, url, header));
}