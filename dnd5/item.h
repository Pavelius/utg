#pragma once
#include "dice.h"
#include "nameable.h"
#include "wear.h"

struct itemi : nameable {
	dice			damage;
	int				count, cost, weight;
	wear_s			wear;
};
struct item {
	unsigned char	type, subtype, count;
	struct {
		unsigned char identified : 1;
		unsigned char broken : 2;
		unsigned char charge : 5;
	};
	explicit operator bool() const { return type != 0; }
	void			add(item& v);
	void			clear();
	void			create(const char* id, int count = 1);
	void			create(const itemi* pi, int count = 1);
	int				getcost() const;
	int				getcostall() const;
	int				getcount() const;
	dice			getdamage() const;
	const char*		getfullname(int price_percent = 0) const;
	const itemi&	geti() const;
	const char*		getname() const;
	int				getweight() const;
	bool			is(wear_s v) const;
	bool			is(const itemi* pv) const;
	bool			iscountable() const { return geti().count != 0; }
	void			setcount(int v);
	void			use();
};
