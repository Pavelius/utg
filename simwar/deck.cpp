#include "deck.h"

BSDATA(decki) = {
	{"TacticCards"},
};
assert_enum(decki, TacticCards);

void deck::create(deck_s v) {
	auto source = bsdata<decki>::elements + v;
	for(auto& e : bsdata<cardi>()) {
		if(e.deck == source)
			addall(e);
	}
}

void deck::matchmask(const cardaf& source, bool keep) {
	auto pb = begin();
	for(auto p : *this) {
		auto i = bsdata<cardi>::source.indexof(p);
		if(i == -1)
			continue;
		if(source.is(i) != keep)
			continue;
		*pb++ = p;
	}
	count = pb - begin();
}

void deck::addall(cardi& v) {
	for(auto i = 0; i < v.count; i++)
		add(&v);
}