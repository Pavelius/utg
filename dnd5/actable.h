#pragma once
#include "gender.h"
#include "variant.h"

class actable {
	variant			kind;
	gender_s		gender;
	short unsigned	name_id;
public:
	static void		actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s female, char separator);
	void			act(const char* format, ...) const;
	variant			getkind() const { return kind; }
	const char*		getname() const;
	static const char* getname(const void* object) { return ((actable*)object)->getname(); }
	bool			ischaracter() const;
	bool			iskind(variant v) const;
	bool			isnamed() const { return name_id != 0xFFFF; }
	void			setgender(gender_s v) { gender = v; }
	void			setkind(variant v) { kind = v; }
	void			setnoname() { name_id = 0xFFFF; }
	void			setname(unsigned short v) { name_id = v; }
};