#pragma once

enum gendern : unsigned char;

struct actable {
	gendern			gender;
	unsigned short	name;
	void			actv(const char* format, const char* format_param) const;
	void			act(const char* format, ...) const;
	const char*		getname() const;
};
