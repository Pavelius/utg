#pragma once

struct abilityi {
	enum { Stat, Indicator };
	const char*		id;
	unsigned		flags;
	constexpr bool	is(unsigned v) const { return (flags & (1 << v)) != 0; }
};
