#include "crt.h"
#include "stringbuilder.h"

#pragma once

enum harm_s : unsigned char {
	Injury, Exhaustion, Wear, Depletion,
	Morale, Value,
};
struct harmi {
	const char*	id;
};
struct harmable {
	typedef slice<harm_s> harma;
	char	harm[Value + 1];
	void	add(harm_s v, int i) { harm[v] += i; }
	void	clear();
	harm_s	getdefault() const;
	int		getdistinct() const;
	int		getdistinct(const harma& source) const;
	void	getinfo(stringbuilder& sb, const harma& source) const;
	void	getinfo(stringbuilder& sb) const;
};
extern harmable inflict, suffer;