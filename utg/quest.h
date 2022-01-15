#include "variant.h"

#pragma once

struct quest {
	typedef bool (*fnallow)(variant v);
	short				index, next;
	const char*			text;
	variants			tags;
	constexpr explicit operator bool() { return text != 0; }
	void				clear();
	bool				isanswer() const { return next != -1; }
	static const quest*	findprompt(short id, fnallow proc);
	static void			read(const char* url);
};
