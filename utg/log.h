#pragma once

namespace log {
struct contexti {
	const char*	url;
	const char*	file;
};
typedef void (*fnread)(const char* url);
typedef void (*fnprint)(const char* format);
extern bool		allowparse;
extern fnprint	print_proc;
extern int		error_count;
extern contexti	context;
void			close();
int				geterrors();
int				getline(const char* pb, const char* pc);
void			error(const char* position, const char* format, ...);
void			errorv(const char* position, const char* format, const char* format_param);
void			print(const char* format, ...);
void			println();
void			printv(const char* format);
void			printv(const char* format, const char* format_param);
const char*		read(const char* url, bool error_if_not_exist = true);
void			readloc(fnread proc, const char* folder, const char* id);
void			readlocfolder(fnread proc, const char* folder, const char* filter);
void            setfile(const char* v);
void            seturl(const char* v);
const char*		skipws(const char* p);
const char*		skipwscr(const char* p);
}
