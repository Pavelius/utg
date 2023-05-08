#pragma once

enum gender_s : unsigned char;

struct actable {
	gender_s		gender;
	unsigned short	name;
	void			actv(const char* format, const char* format_param) const;
	void			act(const char* format, ...) const;
	const char*		getname() const;
};
