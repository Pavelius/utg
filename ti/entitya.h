#include "collection.h"
#include "entity.h"

#pragma once

struct entitya : public collection<entity> {
	void			addreach(const systemi* system, int range);
	entity*			choose(const char* title, const char* cancel = 0, int choose_mode = 0) const;
	entity*			getbest(indicator_s v) const;
	int				getcap() const;
	int				getproductioncost() const;
	int				getsummary(ability_s v) const;
	int				getsummary(indicator_s v) const;
	int				getsummary(unit_type_s v) const;
	int				getsummary(const uniti* pv) const;
	int				getfight() const;
	void			ingame();
	bool			have(entity* v) const { return find(v) != -1; }
	void			match(fnallow proc, int param, bool keep) { collectiona::match(proc, param, keep); }
	void			match(fnvisible proc, bool keep) { collectiona::match(proc, keep); }
	void			match(ability_s id, int value, bool keep);
	void			match(const playeri* player, bool keep);
	void			match(const systemi* system, bool keep);
	void			match(unit_type_s type, bool keep);
	void			match(planet_trait_s value, bool keep);
	void			match(color_s value, bool keep);
	void			match(tag_s value, bool keep);
	void			match(indicator_s value, bool keep);
	void			matchload(bool keep);
	void			matchmove(int mode, bool keep);
	void			matchrange(int range, bool keep);
	void			select(array& source);
	void			select(array& source, fnvisible proc, bool keep) { collectiona::select(source, proc, keep); }
	void			select(array& source, fnallow proc, int param, bool keep) { collectiona::select(source, proc, param, keep); }
	void			select(answers& an);
	void			select(const entity* location);
	void			select(const playeri* player, const entity* system, unit_type_s type);
	void			select(const playeri* player, const entity* location);
	void			selectplanets(const systemi* system);
	void			shuffle();
	void			sortunit();
};