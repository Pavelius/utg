#include "collection.h"
#include "interval.h"
#include "duration.h"
#include "flagable.h"
#include "nameable.h"
#include "range.h"
#include "variant.h"

#pragma once

enum abilityn : unsigned char;
enum spelln : unsigned char;

typedef flagable<8> spellf;
struct spelli : nameable {
	typedef adat<spelln, 8> spella;
	char		level[4];
	durationn	duration;
	rangen		range;
	interval	random;
	spella		dispell;
	spelln		reversed, version, enchant;
	variants	wearing, instant, filter;
	variant		summon;
	spelln		getbasic() const { return reversed ? reversed : version; }
	bool		isdurable() const { return duration != Instant; }
	bool		isevil() const;
};
struct spellable {
	unsigned char spells[128];
	void		clear();
};
struct spella : collection<spelli> {
	void		select(const spellf& source);
	void		select(const spellable& source);
	void		select(int magic, int level);
	void		match(int magic, int level, bool keep);
};
extern spelln last_spell;
extern int last_level;

bool spell_effect(spelln spell, int level, rangen range, const interval& target, const char* suffix, bool run);
bool spell_effect(spelln spell, int level, const char* suffix, bool run);