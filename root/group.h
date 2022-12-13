#pragma once

enum group_s : unsigned char {
	Individual, SmallGroup, MediumGroup, LargeGroup
};
struct groupi {
	const char*	id;
	int			minimal;
};