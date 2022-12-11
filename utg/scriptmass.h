#pragma once

union variant;

struct scriptmass {
	typedef void(*fnproc)(variant v);
	const char* id;
	fnproc		proc;
};