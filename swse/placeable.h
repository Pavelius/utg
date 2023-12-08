#pragma once

struct area;
struct placeable {
	short unsigned place;
	area*	getplace() const;
	void	setplace(const area* v);
};
