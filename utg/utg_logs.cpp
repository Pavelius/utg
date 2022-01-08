#include "recordset.h"
#include "utg.h"

const char* logs::url;
const char* logs::header;
bool logs::interactive;

const char* logs::getchoose(const char* id) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("Choose%1", id);
	return getnm(temp);
}

int logs::choose(const array& source, const char* title, const char* cancel) {
	return source.indexof(recordset::choose(source, title, cancel, interactive, url, header));
}

int logs::choose(const answers& source, const char* title, const char* cancel) {
	return (int)source.choose(title, cancel, interactive, url, -1, header);
}

int logs::choose(const array& source, const flaga& mask, const char* title, const char* cancel) {
	recordset records;
	for(auto i = 0; i < mask.getmaximum(); i++) {
		if(!mask.is(i))
			continue;
		records.add(source.ptr(i));
	}
	return source.indexof(records.choose(title, cancel, interactive, url, header));
}