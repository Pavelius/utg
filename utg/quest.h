#include "variant.h"

#pragma once

struct quest {
	short			index, next;
	const char*		text;
	variants		tags;
	constexpr explicit operator bool() { return text != 0; }
	void			clear();
	const char*		getheader() const;
	const char*		getimage() const;
	int				getvalue(int index) const;
	bool			is(variant v) const;
	bool			isanswer() const { return next != -1; }
	static void		manual(short page);
};
extern const quest* last_quest;

void quest_initialize();
void quest_read(const char* url);
void quest_run(int index);

const char*	quest_text(int id);

const quest* quest_find(short id);
const quest* quest_find_prompt(short id);