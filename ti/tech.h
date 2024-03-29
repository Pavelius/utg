#include "collection.h"
#include "flagable.h"
#include "nameable.h"
#include "stringbuilder.h"
#include "techcolor.h"

#pragma once

enum tech_s : unsigned char {
	PlasmaScoring, MagenDefenseGrid, DuraniumArmor, AssaultCannon,
	NeuralMotivator, DacxiveAnimators, HyperMetabolism, X89BacterialWeapon,
	AntimassDeflectors, GravityDriver, FleetLogistics, LightWaveDeflector,
	SarweenTools, GravitonLaserSystem, TransitDiodes, IntegratedEconomy
};

struct playeri;

typedef flagable<8> techa;

struct requirement {
	char			required[4]; // RGBY
	bool			match(const requirement& e) const;
	int				maximize() const;
};
struct techi : nameable {
	color_s			color;
	char			required[4]; // RGBY
	playeri*		player;
	void			getinfo(stringbuilder& sb) const;
	static void		getinfo(const void* object, stringbuilder& sb) { ((techi*)object)->getinfo(sb); }
	bool			match(const requirement& e) const;
};
extern collection<techi> technologies;