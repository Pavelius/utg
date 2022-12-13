#include "tag.h"
#include "move.h"

#pragma once

struct tagable {
	flagable<4> tags;
	movea		moves;
	void add(tag_s v) { tags.set(v); }
	void add(move_s v) { moves.set(v); }
	constexpr bool is(tag_s v) const { return tags.is(v); }
	constexpr bool is(move_s v) const { return moves.is(v); }
};
