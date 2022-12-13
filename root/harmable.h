#include "crt.h"
#include "stringbuilder.h"

#pragma once

enum harm_s : unsigned char {
	Injury, Exhaustion, Wear, Depletion,
	Morale, Value,
};
struct harmi {
	const char*		id;
};
struct harmable {
	typedef slice<harm_s> harma;
	char	harm[Value + 1];
	void	add(harm_s v, int i) { harm[v] += i; }
	void	clear();
	harm_s	getdefault() const;
	int		getdistinct() const;
	int		getdistinct(harma source) const;
	void	getinfo(stringbuilder& sb, harma source) const;
	void	getinfo(stringbuilder& sb, const char* prompt) const;
};
