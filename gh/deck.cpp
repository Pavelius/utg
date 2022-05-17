#include "answers.h"
#include "deck.h"

cardt deck::take() {
	if(count)
		return data[--count];
	return 0;
}

void deck::discard(cardt v) {
	if(count >= getmaximum())
		return;
	memmove(data + 1, data, count*sizeof(data[0]));
	data[0] = v;
	count++;
}

cardt deck::choose(const char* title, const array& source) {
	struct element {
		const char* id;
	};
	answers an;
	for(auto v : *this)
		an.add((void*)v, getnm(((element*)source.ptr(v))->id));
	return (cardt)(int)an.choose(title);
}