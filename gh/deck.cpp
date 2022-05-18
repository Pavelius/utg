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

cardt deck::choose(const char* title, const array& source, bool need_remove) {
	struct element {
		const char* id;
	};
	answers an;
	for(auto& v : *this) {
		auto p = (element*)source.ptr(v);
		an.add(p, getnm(p->id));
	}
	an.sort();
	auto p = (element*)an.choose(title);
	auto v = (cardt)source.indexof(p);
	auto i = find(v);
	if(i!=-1 && need_remove)
		remove(i, 1);
	return v;
}