#include "scope.h"

scopei*	current_scope;
int	locale_maximum;

scopei::scopei(int value) {
	scope = value;
	size = 0;
	previous = current_scope;
	current_scope = this;
}

scopei::~scopei() {
	auto n = getsize();
	if(locale_maximum < n)
		locale_maximum = n;
	current_scope = previous;
}

int scopei::getsize() const {
	auto r = 0;
	for(auto p = current_scope; p; p = p->previous)
		r += p->size;
	return r;
}