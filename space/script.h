#include "stringbuilder.h"

enum {
	AnswersRoll = 20,
	AnswersForward = 1000,
};

#pragma once

struct scripti {
	typedef void (*fnevent)(int counter, int param);
	typedef void (*fninfo)(stringbuilder& sb, const scripti& e, int counter);
	typedef bool (*fntest)(int counter, int param);
	const char*	id;
	fnevent		proc;
	int			param;
	void		apply(int bonus) const;
};
void			actv(stringbuilder& sb, const char* format, const char* format_param, char add_sep = '\n'); // Export action logger