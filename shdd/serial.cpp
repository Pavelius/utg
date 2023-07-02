#include "archive.h"
#include "creature.h"
#include "location.h"
#include "lyingitem.h"

static void game_serial(archive& a) {
	a.set(bsdata<locationi>::source);
	a.set(bsdata<creature>::source);
	a.set(bsdata<lyingitem>::source);
}