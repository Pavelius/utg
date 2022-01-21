#pragma once

struct avatarable {
	char			avatar[16];
	explicit operator bool() const { return avatar[0] != 0; }
	const char*		getavatar() const { return avatar; }
	void			setavatar(const char* id);
};
