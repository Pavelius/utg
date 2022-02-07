#pragma once

class stringbuilder;

struct scripti {
	typedef void (*fnevent)(int counter, int param);
	typedef bool (*fntest)(int counter, int param);
	const char*		id;
	fnevent			proc;
	int				param;
	fntest			choose;
};
