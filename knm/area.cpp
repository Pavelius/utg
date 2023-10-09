#include "area.h"
#include "province.h"
#include "crt.h"

const short unsigned Blocked = 0xFF;
const short unsigned NotCalculatedMovement = 0xFE;

areai area;

static point stack[256 * 256];
static point* push_counter;
static point* pop_counter;
static areai movement_rate;
static point straight_directions[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
static point all_directions[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

static void addwave(point v) {
	*push_counter++ = v;
	if(push_counter >= stack + sizeof(stack) / sizeof(stack[0]))
		push_counter = stack;
}

static point getwave() {
	auto index = *pop_counter++;
	if(pop_counter >= stack + sizeof(stack) / sizeof(stack[0]))
		pop_counter = stack;
	return index;
}

point areai::getdirection(point hex, int direction) {
	static point evenr_directions[2][6] = {
		{{+1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, +1}, {+1, +1}},
		{{+1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, +1}, {0, +1}},
	};
	auto parity = hex.y & 1;
	auto offset = evenr_directions[parity][direction];
	return hex + offset;
}

void areai::blockzero() {
	point m;
	for(m.y = 0; m.y < mps; m.y++)
		for(m.x = 0; m.x < mps; m.x++) {
			if(movement_rate[m] >= NotCalculatedMovement)
				movement_rate[m] = Blocked;
		}
}

void areai::clear() {
	memset(this, 0, sizeof(*this));
	point m;
	for(m.y = 0; m.y < mps; m.y++)
		for(m.x = 0; m.x < mps; m.x++)
			set(m, Blocked);
}

unsigned char areai::getblock(point m) {
	return movement_rate[m];
}

void areai::clearpath() {
	point m;
	for(m.y = 0; m.y < mps; m.y++)
		for(m.x = 0; m.x < mps; m.x++)
			movement_rate[m] = NotCalculatedMovement;
	push_counter = stack;
	pop_counter = stack;
}

point areai::getnext(point start, point goal) const {
	auto n = getpath(start, goal, stack, sizeof(stack) / sizeof(stack[0]));
	if(!n)
		return {-1000, -1000};
	return stack[n - 1];
}

unsigned areai::getpath(point start, point goal, point* result, unsigned maximum) const {
	auto pb = result;
	auto pe = result + maximum;
	auto curr = goal;
	auto cost = 0xFFFF;
	if(pb == pe || curr == start)
		return 0;
	*pb++ = goal;
	while(pb < pe) {
		auto next = curr;
		for(auto i = 0; i < 6; i++) {
			auto i1 = getdirection(curr, i);
			if(!isvalid(i1))
				continue;
			if(i1 == start) {
				next = i1;
				break;
			}
			auto c1 = movement_rate[i1];
			if(c1 >= cost)
				continue;
			next = i1;
			cost = c1;
		}
		if(next == curr || next == start)
			break;
		*pb++ = next;
		curr = next;
	}
	return pb - result;
}

void areai::setblock(point m, mt v) {
	if(isvalid(m))
		movement_rate[m] = v;
}

void areai::set(point m, mt v) {
	if(isvalid(m))
		operator[](m) = v;
}

void areai::makewavex() {
	while(pop_counter != push_counter) {
		auto m = getwave();
		auto cost = movement_rate[m] + 1;
		for(auto i = 0; i < 6; i++) {
			auto m1 = getdirection(m, i);
			if(!isvalid(m1))
				continue;
			auto c1 = movement_rate[m1];
			if(c1 == Blocked || c1 <= cost)
				continue;
			movement_rate[m1] = cost;
			addwave(m1);
		}
	}
	blockzero();
}

void areai::makewave(point start_index) {
	movement_rate[start_index] = 0;
	addwave(start_index);
	makewavex();
}

void areai::blockrange(int range) {
	point m;
	for(m.y = 0; m.y < mps; m.y++)
		for(m.x = 0; m.x < mps; m.x++) {
			auto v = movement_rate[m];
			if(v == Blocked)
				continue;
			if(v > range)
				movement_rate[m] = Blocked;
		}
}