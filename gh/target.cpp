#include "main.h"

BSDATA(targeti) = {
	{"TargetAttack"},
	{"TargetSelf"},
	{"TargetAlly"},
	{"TargetEnemiesAround"},
	{"TargetAlliesAround"},
	{"TargetCreaturesAround"},
};
assert_enum(targeti, TargetCreaturesAround)