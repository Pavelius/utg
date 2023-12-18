#include "collection.h"
#include "dice.h"
#include "duration.h"
#include "flagable.h"
#include "nameable.h"
#include "range.h"

#pragma once

enum spell_s : unsigned char {
	CauseLightWound, CauseFear, CureLightWound, Darkness, DetectEvil, DetectMagic, Light, ProtectionFromEvil, PurifyFoodAndWater, RemoveFear, ResistCold,
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
	char		level[4];
	duration_s	duration;
	range_s		range;
	dice		effect;
	spell_s		dispell[4], mass_effect;
	dice		hds;
	void		dispelling() const;
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
extern spelli* last_spell;