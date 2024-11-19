#include "creature.h"
#include "bsdata.h"
#include "organization.h"

BSDATA(organizationi) = {
	{"MiddleKindom"},
	{"Rebels"},
	{"Horde"},
	{"DwarvenKindom"},
	{"ElvenKindom"},
	{"LawfulGod"},
	{"ChaoticGod"},
	{"NatureGod"},
	{"TheivesGuild"},
	{"MercenaryGuild"},
	{"MerchantsGuild"},
	{"WizardCouncil"},
};
BSDATAF(organizationi)

BSDATA(orgstati) = {
	{"Reputation"},
	{"Influence"},
};
assert_enum(orgstati, Influence)

void start_game_organization() {
	for(auto& e : bsdata<organizationi>()) {
		e.setowner(0);
	}
}

void start_game_reputation() {
	for(auto& e : bsdata<organizationi>()) {
		auto index = getbsi(&e);
		for(auto p : creatures) {
			if(p->geti().reputation.is(index))
				e.abilities[Reputation]++;
		}
	}
}