#pragma once

class stringbuilder;

struct scripti {
	typedef void (*fnevent)(int counter, int param);
	typedef void (*fninfo)(stringbuilder& sb, const scripti& e, int counter);
	const char*		id;
	fnevent			proc;
	int				param;
	fninfo			info;
	static void		add(const char* id, fnevent proc, int param = 0, fninfo info = 0);
	void			getinfo(stringbuilder& sb, int counter) const { info(sb, *this, counter); }
};
