#include "bsdata.h"
#include "flagable.h"
#include "tag.h"

BSDATA(tagi) = {
	{"ArrowProof", FG(Positive)},
	{"Blunted", FG(Positive)},
	{"CatfolkSteel", FG(Positive)},
	{"Comfortable", FG(Positive)},
	{"Cumbersome", FG(Negative)},
	{"Durable", FG(Positive)},
	{"Eaglecraft", FG(Positive)},
	{"Flexible", FG(Positive)},
	{"Large"},
	{"Makeshift"},
	{"MetalBoss"},
	{"Precise", FG(Positive)},
	{"Quick"},
	{"Reach"},
	{"Reinforced", FG(Positive)},
	{"Sharp", FG(Positive)},
	{"Shoddy", FG(Negative)},
	{"SteelOfRabbitfolk", FG(Positive)},
	{"SteelOfFoxfolk", FG(Positive)},
	{"TightlyWoven", FG(Positive)},
	{"Throwable", FG(Positive)},
	{"Unassuming", FG(Positive)},
	{"Weighty", FG(Negative)},
	{"Positive"},
	{"Negative"},
	{"Load"},
	{"MarkWear"},
	{"MarkExhaustion"},
	{"Armor"},
	{"Intimate"},
	{"Close"},
	{"Far"},
};
assert_enum(tagi, Far)