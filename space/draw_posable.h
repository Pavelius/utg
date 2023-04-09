#include "crt.h"
#include "point.h"

#pragma once

struct posable {
	typedef adat<posable*, 512> collection_type;
	typedef void(*fnpaint)(const posable* p);
	typedef void(*fnget)(collection_type& e);
	point				position;
	unsigned char		alpha, priority; // priority==0 for empthy object
	explicit operator bool() const { return priority != 0; }
	void				clear();
	void				focusing() const;
	static fnget		getposables;
	static rect			getscreen(int offset);
	bool				iswaitable() const;
	void				paint() const;
	static void			paintall();
	static fnpaint		painting;
	static void			slide(point goal, int step);
	static void			splash(unsigned milliseconds);
	void				wait() const;
	static void			waitall();
};