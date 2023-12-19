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

void print(char separator, const char* format, ...);
bool printa(const char* id, const char* s1 = 0, char separator = '\n');
void printn(const char* format, ...);
void prints(const char* format, ...);
void printv(char separator, const char* format, const char* format_param);