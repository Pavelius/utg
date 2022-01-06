#pragma once

class avatarable {
	char			avatar[16];
public:
	const char*		getavatar() const { return avatar; }
	void			setavatar(const char* id);
};
