#include "variant.h"

#pragma once

struct quest {
	short				index, next;
	const char*			text;
	const char*			image;
	variants			tags;
	static stringbuilder* console;
	constexpr explicit operator bool() { return text != 0; }
	void				clear();
	const quest*		choose(int id, const char* resid, const char* header) const;
	bool				isanswer() const { return next != -1; }
	static const quest*	findprompt(short id);
	static void			read(const char* url);
	static void			run(int id, const char* resid, const char* header);
};
