#pragma once

typedef void (*fnevent)();

namespace code {
struct command {
	const char* id;
	fnevent proc;
};
}