#pragma once

class avatarable {
	char			avatar[16];
public:
	explicit operator bool() const { return avatar[0] != 0; }
	const char*		getavatar() const { return avatar; }
	void			setavatar(const char* id);
};
