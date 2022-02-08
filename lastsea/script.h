#pragma once

class stringbuilder;

struct scripti {
	enum {
		TipsInfo,
	};
	typedef void (*fnevent)(int counter, int param);
	typedef bool (*fntest)(int counter, int param);
	const char*		id;
	fnevent			proc;
	int				param;
	fntest			choose;
	unsigned		flags;
	void			getinfo(stringbuilder& sb, int bonus) const;
	constexpr bool	is(int v) const { return (flags & (1 << v)) != 0; }
};
