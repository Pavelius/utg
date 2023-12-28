#include "collection.h"
#include "interval.h"
#include "duration.h"
#include "flagable.h"
#include "nameable.h"
#include "range.h"

#pragma once

enum ability_s : unsigned char;

enum spell_s : unsigned char {
	CauseLightWound, CureLightWound, DetectEvil, DetectMagic, Light, LightBlindness, Darkness, ProtectionFromEvil, PurifyFoodAndWater, RemoveFear, CauseFear, ResistCold,
	CharmPerson, FloatingDisc, HoldPortal, MagicMissile, ReadLanguages, ReadMagic, Shield, Sleep, Ventriloquism,
	Blindness, ContinualDarkness, ContinualLight, DetectInvisibility, ESP, Invisibility, Knock, Levitation, MirrorImages, PhantasmalForce, Web, WizardLock,
	Bless, Blight, FindTraps, HoldPerson, KnowAlignment, ResistFire, Silence15Radius, SnakeCharm, SpeakWithAnimals,
	BestowCurse, CauseDisease, CureDisease, GrowthOfAnimals, LocateObject, RemoveCurse, FlameBlade,
	AntiMagicShell, Teleport, DeathSpell,
	LastSpell = DeathSpell,
	AnimateTree, AnkhegAcidSquirt, BeetleOilOfPain, ConeOfFire, ItemRepair,
	ShrinkSize, GrowthSize, GaseousForm, DeathPoison,
};
typedef flagable<8> spellf;
struct spelli : nameable {
	typedef adat<spell_s, 8> spella;
	char		level[4];
	duration_s	duration;
	range_s		range;
	interval	count, targets;
	spella		dispell;
	ability_s	save_negates, save_halves;
	spell_s		reversed, alternate, enchant;
	void		dispelling() const;
	spell_s		getbasic() const { return reversed ? reversed : alternate; }
	ability_s	getsave() const;
	bool		isdurable() const { return duration != Instant; }
	bool		isevil() const;
};
struct spellable {
	unsigned char spells[DeathSpell + 1];
	void		clear();
};
struct spella : collection<spelli> {
	void		select(const spellf& source);
	void		select(const spellable& source);
	void		select(int magic, int level);
	void		match(int magic, int level, bool keep);
};
extern spell_s last_spell;
extern int last_level;

bool spell_effect(spell_s spell, int level, range_s range, const interval& target, const char* suffix, bool run);
bool spell_effect(spell_s spell, int level, const char* suffix, bool run);