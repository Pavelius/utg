#include "main.h"

npcname& party::getactor(int v) {
	unsigned i = data[((unsigned)v) % maxcount];
	if(i >= 1 && i < maxcount)
		return pirates[i - 1];
	return *this;
}

void party::shuffleparcipant() {
	for(unsigned i = 0; i < maxcount; i++)
		data[i] = i;
	zshuffle(data, maxcount);
}

void party::generate() {
	for(auto& e : pirates) {
		e.clear();
		e.randomname();
	}
	pirate::clear();
	pirate::randomname();
	pirate::generate();
}