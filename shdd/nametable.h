#include "nameable.h"
#include "variant.h"

#pragma once

struct nametable : nameable {
	struct element {
		short		weight;
		const char*	name;
		variants	elements;
	};
	slice<element>	elements;
	const element*	first(int value) const;
	int				getrange() const;
	const element*	random() const;
	static void		read(const char* url);
};
