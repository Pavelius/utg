#include "main.h"

BSDATA(targeti) = {
	{"TargetAttack"},
	{"TargetSelf"},
	{"TargetAlly"},
	{"TargetEnemiesAround"},
	{"TargetAllyAround"},
	{"TargetAllyEnemyAround"},
};
assert_enum(targeti, TargetAllyEnemyAround)