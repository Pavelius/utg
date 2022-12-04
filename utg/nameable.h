#pragma once

struct nameable {
	const char*	id;
	const char*	getid() const { return id; }
	const char*	getname() const;
	static const char* getname(const void* object) { return ((nameable*)object)->getname(); }
};
