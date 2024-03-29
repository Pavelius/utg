#include "variant.h"

#pragma once

struct stringlist {
	const char*			id;
	const char*			name;
	short unsigned		index, weight;
	unsigned			getcount() const;
	const char*			get() const;
	static const char*	getname(unsigned short v);
	bool				match(const char* id) const;
	static short unsigned random(const char* id);
	static void			read(const char* url);
	static unsigned		select(short unsigned* pb, short unsigned* pe, const char* id);
};