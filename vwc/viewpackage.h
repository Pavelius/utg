#include "code_package.h"

#pragma once

namespace code {
struct viewpackage : package {
	const char*		source;
	unsigned long	stamp;
	unsigned		flags;
	void			clear();
};
extern const char	*project, *library;
viewpackage*		openview(const char* id);
}
