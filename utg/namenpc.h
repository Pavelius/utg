#pragma once

struct namenpc {
	short unsigned	name_id;
	constexpr explicit operator bool() const { return name_id != 0xFFFF; }
	void			clearname() { name_id = 0xFFFF; }
	const char*		getname() const;
};
