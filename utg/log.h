#pragma once

namespace log {
typedef void (*fnread)(const char* url);
extern bool allowparse;
struct contexti {
	const char*	url;
	const char*	file;
};
extern contexti	context;
void			close();
int				geterrors();
void			error(const char* position, const char* format, ...);
void			errorv(const char* position, const char* format);
const char*		read(const char* url, bool error_if_not_exist = true);
void			readloc(fnread proc, const char* folder, const char* id);
void            setfile(const char* v);
void            seturl(const char* v);
const char*		skipws(const char* p);
const char*		skipwscr(const char* p);
}
