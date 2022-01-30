#include "variant.h"

#pragma once

struct quest {
	short				index, next;
	const char*			text;
	const char*			image;
	const char*			header;
	variants			tags;
	static stringbuilder* console;
	constexpr explicit operator bool() { return text != 0; }
	static void			apply(const variants& source);
	void				clear();
	const quest*		choose(int id, const char* resid, const char* header) const;
	static const char*	getname(int id);
	bool				is(variant v) const;
	bool				isanswer() const { return next != -1; }
	static const quest*	find(short id);
	static const quest*	findprompt(short id);
	static void			read(const char* url);
	static void			run(int id, const char* resid, const char* header);
};
