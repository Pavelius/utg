#include "crt.h"
#include "nameable.h"
#include "tagable.h"

#pragma once

struct itemi : nameable, tagable {
	char			wear, load;
	constexpr bool isweapon() const { return is(Far) || is(Close) || is(Intimate); }
};
struct item {
	unsigned char type;
	unsigned char usable : 1;
	unsigned char disarmed : 1;
	unsigned char wear : 3;
	unsigned char wear_max : 3;
	unsigned char exhaused : 1;
	unsigned char marked : 1;
	constexpr explicit operator bool() const { return type != 0; }
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			create(unsigned char type);
	void			damage(int v);
	void			destory();
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
	int				getload() const;
	const char*		getname() const;
	bool			is(tag_s v) const { return geti().tags.is(v); }
};