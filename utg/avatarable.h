#pragma once

struct avatarable {
	char			avatar[16];
	explicit operator bool() const { return avatar[0] != 0; }
	void			clear();
	static const char* choose(const char* title, const char* mask);
	const char*		getavatar() const { return avatar; }
	void			setavatar(const char* id);
};
