#pragma once

struct filehandle {
	typedef void (*fnhandle)(const char* url);
	const char*		id;
	const char*		filter;
	fnhandle		open, save;
};
extern char current_file[260];