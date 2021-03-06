#include "answers.h"

#pragma once

class chooselist : public answers {
	adat<char, 32> marked;
public:
	chooselist() : marked() {}
	virtual void	apply(int index, const void* object) {}
	void			addmarked(int index);
	bool			choose(const char* title, int count, const char* cancel = 0);
	int				getmarked(int v) const;
	int				getvalueindex(const void* pv) const;
	virtual bool	isallow(int index, const void* object) const;
	bool			ismarked(int index) const { return getmarked(index) != 0; }
};