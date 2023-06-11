#pragma once

struct avatarable {
	typedef bool(*fnallow)(const char* name);
	char			avatar[16];
	explicit operator bool() const { return avatar[0] != 0; }
	void			clear();
	static const char* choose(const char* title, const char* mask, int columns_count, fnallow allow = 0);
	const char*		getavatar() const { return avatar; }
	void			setavatar(const char* id);
};
