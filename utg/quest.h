#include "variant.h"

#pragma once

struct quest {
	typedef bool (*fnallow)(variant v);
	short				index, next;
	const char*			text;
	const char*			image;
	variants			tags;
	static fnallow		allow;
	static fnallow		apply;
	constexpr explicit operator bool() { return text != 0; }
	void				clear();
	const quest*		choose(int id, const char* title, const char* resid, const char* header) const;
	bool				isanswer() const { return next != -1; }
	static const quest*	findprompt(short id);
	static void			read(const char* url);
	static void			run(int id, const char* title, const char* resid, const char* header);
};
