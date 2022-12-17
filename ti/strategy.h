#include "nameable.h"
#include "variant.h"

#pragma once

struct strategyi : nameable {
	int			initiative;
	variants	primary, secondary;
	void		getinfo(stringbuilder& sb) const;
};