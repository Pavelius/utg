#include "answers.h"
#include "deck.h"
#include "nameable.h"

short unsigned deck::take() {
	if(count)
		return data[--count];
	return 0;
}

void deck::discard(short unsigned v) {
	if(count >= getmaximum())
		return;
	memmove(data + 1, data, count*sizeof(data[0]));
	data[0] = v;
	count++;
}

short unsigned deck::choose(const char* title, const array& source, bool need_remove) {
	answers an;
	for(auto& v : *this) {
		auto p = (nameable*)source.ptr(v);
		an.add(p, getnm(p->id));
	}
	an.sort();
	auto p = (nameable*)an.choose(title);
	auto v = source.indexof(p);
	auto i = find(v);
	if(i!=-1 && need_remove)
		remove(i, 1);
	return v;
}