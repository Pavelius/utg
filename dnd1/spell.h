#include "collection.h"
#include "interval.h"
#include "duration.h"
#include "flagable.h"
#include "nameable.h"
#include "range.h"

#pragma once

enum ability_s : unsigned char;

enum spell_s : unsigned char {
	CauseLightWound, CauseFear, CureLightWound, DetectEvil, DetectMagic, Light, Blinding, Darkness, ProtectionFromEvil, PurifyFoodAndWater, RemoveFear, ResistCold,
	CharmPerson, FloatingDisc, HoldPortal, MagicMissile, ReadLanguages, ReadMagic, Shield, Sleep, Ventriloquism,
	Blindness, ContinualDarkness, ContinualLight, DetectInvisibility, ESP, Invisibility, Knock, Levitation, MirrorImages, PhantasmalForce, Web, WizardLock,
	Bless, Blight, FindTraps, HoldPerson, KnowAlignment, ResistFire, Silence15Radius, SnakeCharm, SpeakWithAnimals,
	BestowCurse, CauseDisease, CureDisease, GrowthOfAnimals, LocateObject, RemoveCurse, FlameBlade,
	AntiMagicShell, DeathSpell,
	LastSpell = DeathSpell,
	ShrinkSize, GrowthSize, GaseousForm, DeathPoison,
};
typedef flagable<8> spellf;
struct spelli : nameable {
	typedef adat<spell_s, 8> spella;
	char		level[4];
	duration_s	duration;
	range_s		range;
	interval	effect;
	spella		dispell;
	interval	hds;
	interval	count;
	ability_s	save_negates, save_halves;
	spell_s		reversed, alternate;
	void		dispelling() const;
	spell_s		getbasic() const { return reversed ? reversed : alternate; }
	bool		isdurable() const { return duration != Instant; }
	bool		isevil() const;
};
struct spellable {
	unsigned char spells[128];
};
struct spella : collection<spelli> {
	void		select(const spellf& source);
	void		select(const spellable& source);
};
extern spell_s last_spell;
extern int last_level;