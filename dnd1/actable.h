#pragma once

class stringbuilder;
enum gender_s : unsigned char;

struct actable {
	const char*		name;
	gender_s		gender;
	void			act(const char* format, ...) const;
	bool			actid(const char* prefix, const char* suffix, char separator = '\n') const;
	void			actn(const char* format, ...) const;
	void			actv(stringbuilder& sb, const char* format, const char* format_param, char separator) const;
	const char*		getname() const { return name; }
};
