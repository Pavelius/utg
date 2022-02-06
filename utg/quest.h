#include "property.h"
#include "variant.h"

#pragma once

struct quest {
	short				index, next;
	const char*			text;
	variants			tags;
	constexpr explicit operator bool() { return text != 0; }
	static propertyi::indext prop_image, prop_header;
	static void			apply(const variants& source);
	void				clear();
	const quest*		choose(int id) const;
	const char*			getheader() const { return getstring(getbsi(this), prop_header); }
	const char*			getimage() const { return getstring(getbsi(this), prop_image); }
	static const char*	getname(int id);
	static void			initialize();
	bool				is(variant v) const;
	bool				isanswer() const { return next != -1; }
	static const quest*	find(short id);
	static const quest*	findprompt(short id);
	static void			read(const char* url);
	static void			run(int id);
};
