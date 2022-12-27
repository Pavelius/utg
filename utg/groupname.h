#pragma once

struct groupname {
	const char*	parent;
	const char*	name;
	static const groupname* random(const char* parent);
	static unsigned short randomid(const char* parent);
	static void read(const char* url);
};
