#include "nameable.h"
#include "variant.h"

#pragma once

struct randomizeri : nameable {
	variants		chance;
	static variant	random(const variants& elements, int range = 0, int bonus = 0);
	variant			random() const { return random(chance); }
	variant			random(int bonus) const { return random(chance, 0, bonus); }
	static int		total(const variants& elements);
};
variant single(variant v);
variant single(variant v, int bonus);