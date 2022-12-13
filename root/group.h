#pragma once

enum group_s : unsigned char {
	Individual, Small, Medium, Large
};
struct groupi {
	const char*	id;
	int			minimal;
};