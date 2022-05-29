#include "main.h"

void playera::filter(const playeri* object, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if((p == object) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

playeri* playera::choose(const char* title) {
	return 0;
}