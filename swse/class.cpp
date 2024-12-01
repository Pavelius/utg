#include "class.h"

classn last_class;

int	classable::getlevel() const {
	auto r = 0;
	for(auto v : classes)
		r += v;
	return r;
}

int	classable::gethlevel() const {
	auto r = 0;
	for(auto i = (classn)Jedi; i <= Soldier; i = (classn)(i + 1))
		r += classes[i];
	return r;
}