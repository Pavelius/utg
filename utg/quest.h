#include "variant.h"

#pragma once

struct questheader;
struct quest {
	short				index, next;
	const char*			text;
	variants			tags;
	constexpr explicit operator bool() { return text != 0; }
	static const quest* last;
	static int			prop_image, prop_header;
	void				clear();
	static const quest*	find(short id);
	static const quest*	findprompt(short id);
	const char*			getheader() const;
	const char*			getimage() const;
	static const char*	getname(int id);
	const questheader*	getquest() const;
	int					getvalue(int prop) const;
	static void			initialize();
	bool				is(variant v) const;
	bool				isanswer() const { return next != -1; }
	static void			manual(short page);
	static void			read(const char* url);
	static void			run(int index);
};
struct questheader {
	const char*			name;
	slice<quest>		source;
};
extern questheader* last_questheader;