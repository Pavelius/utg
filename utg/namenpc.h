#include "gender.h"
#include "stringbuilder.h"

#pragma once

class namenpc {
	short unsigned	nameid;
public:
	void			act(stringbuilder& sb, const char* format, ...) const { actv(sb, format, xva_start(format)); }
	static void		actv(stringbuilder& sb, const char* format, const char* format_param);
	void			clearname() { nameid = 0xFFFF; }
	constexpr bool	isvalidname() const { return nameid != 0xFFFF; }
	gendern			getgender() const;
	const char*		getname() const;
	void			setname(short unsigned v) { nameid = v; }
};
