#pragma once

struct groupname {
	const char*	parent;
	const char*	name;
};

void read_group_names(const char* url);
const groupname* random_group_name(const char* parent);
unsigned short random_group_namei(const char* parent);

