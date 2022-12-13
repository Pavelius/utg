#include "npc.h"

npc* opponent;

void npc::setgroup(int v) {
	harm[Injury] += v * 2;
	for(auto& v : inflict.harm)
		v *= (1 + v);
}