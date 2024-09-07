#pragma once

#include "array.h"

class stringa : public array {
	unsigned			find(const char* v, unsigned len) const;
	unsigned			add(const char* v, unsigned len);
public:
	stringa() : array(1) {}
	unsigned			add(const char* v);
	unsigned			find(const char* v) const;
	const char*			get(unsigned v) const { return (v < count) ? (const char*)ptr(v) : ""; }
};
