#include "category.h"
#include "crt.h"

namespace {
struct element {
	int		object;
	int		value;
	explicit operator bool() const { return object != 0; }
	void clear() { memset(this, 0, sizeof(*this)); }
};
}
static adat<element, 1024> categories;

static element* find(int object) {
	for(auto& e : categories) {
		if(e.object == object)
			return &e;
	}
	return 0;
}

static element* addnew() {
	for(auto& e : categories) {
		if(!e)
			return &e;
	}
	auto p = categories.add();
	if(!p)
		p = categories.data;
	return p;
}

static element* findadd(int object) {
	auto p = find(object);
	if(!p) {
		p = addnew();
		p->clear();
		p->object = object;
	}
	return p;
}

void category::set(int object, int value) {
	auto p = findadd(object);
	p->value = value;
}

int category::get(int object) {
	auto p = find(object);
	if(p)
		return p->value;
	return 0;
}

void category::remove(int object) {
	auto p = find(object);
	if(p)
		p->clear();
}