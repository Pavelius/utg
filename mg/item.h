#include "action.h"
#include "bsdata.h"
#include "nameable.h"
#include "tag.h"

#pragma once

struct itemi : nameable {
	char			ob;
	taga			tags;
};
class item {
	unsigned char	type;
	//
	unsigned char	disarmed : 1;
	unsigned char	useful_used : 1;
	actionn		useful : 2;
	unsigned char	charge : 4; // Light armor or Thrown weapon
public:
	constexpr item(unsigned char type) : type(type), disarmed(0), useful_used(0), useful(Attack), charge(1) {}
	explicit operator bool() const { return type != 0; }
	int				getbonus(actionn value) const;
	int				getcost() const;
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
	const char*		getname() const { return geti().getname(); }
	int				getsuccess(actionn value) const;
	const char*		gettext(actionn value) const;
	bool			is(tag_s v) const { return geti().tags.is(v); }
	bool			isready() const { return disarmed == 0 && charge > 0; }
	void			set(actionn v) { useful = v; }
	void			setdisarm(int v) { disarmed = v; }
	void			setuseful(int v) { useful_used = v; }
};
