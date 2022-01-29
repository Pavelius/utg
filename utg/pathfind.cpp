#include "crt.h"
#include "pathfind.h"

using namespace pathfind;

static indext stack[256 * 256];
static indext movement_rate[64 * 64];

int	pathfind::maxcount = 64 * 64;
fnto pathfind::to;

void pathfind::clearpath() {
	if(maxcount)
		memset(movement_rate, 0, maxcount * sizeof(movement_rate[0]));
}

indext pathfind::getmove(indext i) {
	return movement_rate[i];
}

void pathfind::getmove(indext* destination) {
	if(maxcount)
		memcpy(destination, movement_rate, maxcount * sizeof(movement_rate[0]));
}

void pathfind::setmove(const indext* source) {
	if(maxcount)
		memcpy(movement_rate, source, maxcount * sizeof(movement_rate[0]));
}

void pathfind::setmove(indext i, indext v) {
	movement_rate[i] = v;
}

indext pathfind::getfarest(const indext* source) {
	auto nd = 0;
	auto sd = 64000;
	auto ni = Blocked;
	for(auto i = 0; i < maxcount; i++) {
		if(source[i] == Blocked || movement_rate[i] == Blocked)
			continue;
		if(source[i] <= sd && movement_rate[i] >= nd) {
			sd = source[i];
			nd = movement_rate[i];
			ni = i;
		}
	}
	return ni;
}

indext pathfind::getnearest(const indext* source) {
	auto nd = 64000;
	auto sd = 64000;
	auto ni = Blocked;
	for(auto i = 0; i < maxcount; i++) {
		if(source[i] == Blocked || movement_rate[i] == Blocked)
			continue;
		if(source[i] <= sd && movement_rate[i] <= nd) {
			sd = source[i];
			nd = movement_rate[i];
			ni = i;
		}
	}
	return ni;
}

void pathfind::blockzero() {
	for(indext i = 0; i < maxcount; i++) {
		if(!movement_rate[i])
			movement_rate[i] = Blocked;
	}
}

void pathfind::blocknearest(indext index, indext cost) {
	for(int d = 0; d < 6; d++) {
		auto i1 = to(index, d);
		auto c1 = movement_rate[i1];
		if(i1 == Blocked || c1 == Blocked)
			continue;
		if(c1 && c1 < cost)
			continue;
		movement_rate[i1] = cost;
	}
}

void pathfind::makewave(indext start_index) {
	if(!isinitializated())
		return;
	auto stack_end = stack + sizeof(stack) / sizeof(stack[0]);
	auto push_counter = stack;
	auto pop_counter = stack;
	movement_rate[start_index] = Blocked;
	*push_counter++ = start_index;
	while(pop_counter != push_counter) {
		auto index = *pop_counter++;
		if(pop_counter >= stack_end)
			pop_counter = stack;
		auto cost = ((index == start_index) ? 0 : movement_rate[index]) + 1;
		for(int d = 0; d < 6; d++) {
			auto i1 = to(index, d);
			auto c1 = movement_rate[i1];
			if(i1 == Blocked || c1 == Blocked)
				continue;
			if(c1 && c1 < cost)
				continue;
			movement_rate[i1] = cost;
			if(c1 != StopMovementThrought) {
				*push_counter++ = i1;
				if(push_counter >= stack_end)
					push_counter = stack;
			}
		}
	}
	blockzero();
}

void pathfind::blockrange(int range) {
	for(indext i = 0; i < maxcount; i++) {
		auto v = getmove(i);
		if(v == Blocked)
			continue;
		if(v > range)
			setmove(i, Blocked);
	}
}

bool pathfind::isinitializated() {
	return maxcount > 0 && to;
}