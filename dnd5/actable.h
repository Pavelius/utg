#include "gender.h"
#include "variant.h"

#pragma once

class actable {
	variant			kind;
	gendern			gender;
	short unsigned	name_id;
public:
	void			act(const char* format, ...) const;
	variant			getkind() const { return kind; }
	const char*		getname() const;
	static const char* getname(const void* object) { return ((actable*)object)->getname(); }
	bool			ischaracter() const;
	bool			iskind(variant v) const;
	bool			isnamed() const { return name_id != 0xFFFF; }
	void			setgender(gendern v) { gender = v; }
	void			setkind(variant v) { kind = v; }
	void			setnoname() { name_id = 0xFFFF; }
	void			setname(unsigned short v) { name_id = v; }
};