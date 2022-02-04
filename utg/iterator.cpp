#include "iterator.h"

void aindex_remove(indext* data, size_t count, int i) {
	if(i == -1)
		return;
	auto ps = data;
	auto pe = ps + count;
	for(auto p = ps; p < pe; p++) {
		if(*p == i)
			continue;
		*ps++ = *p;
	}
}

bool aindex_is(indext* data, size_t count, int i) {
	if(i == -1)
		return false;
	auto pe = data + count;
	for(auto p = data; p < pe; p++) {
		if(*p == i)
			return true;
	}
	return false;
}