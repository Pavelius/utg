#include "adat.h"
#include "stringbuilder.h"

#pragma once

class answers {
	struct element {
		const void* value;
		const char* text;
	};
	char				buffer[2048];
	stringbuilder		sc;
	adat<element, 32>	elements;
public:
	typedef void(*fnpaint)(int index, const void* value, const char* text, fnevent press_event);
	static fnevent		afterpaint;
	static fnevent		beforepaint;
	static fnpaint		paintcell;
	static bool			interactive;
	static int			column_count;
	static stringbuilder* console;
	static const char*	header;
	static char			hotkeys[20];
	static const char*	prompt;
	static const char*	prompa;
	static const char*	prompi;
	static const char*	resid;
	static bool			show_tips;
	static answers*		last;
	answers() : sc(buffer) {}
	constexpr operator bool() const { return elements.count != 0; }
	void				add(const void* value, const char* name, ...) { addv(value, name, xva_start(name)); }
	void				addv(const void* value, const char* name, const char* format);
	const element*		begin() const { return elements.data; }
	element*			begin() { return elements.data; }
	void*				choose(const char* title = 0, const char* cancel_text = 0, int cancel_mode = 0) const;
	void				clear();
	static int			compare(const void* v1, const void* v2);
	const element*		end() const { return elements.end(); }
	int					getcount() const { return elements.getcount(); }
	const char*			getname(void* v);
	int					indexof(const void* v) const { return elements.indexof(v); }
	void				modal(const char* title, const char* cancel) const;
	void				paintanswers(int columns, const char* cancel_text) const;
	void*				random() const;
	void				remove(int index) { elements.remove(index, 1); }
	void				sort();
};
extern answers an;

void message(const char* format, const char* header = 0);
void pause();
void pause(const char* title, ...);
void pausenc(const char* title, ...);
bool yesno(const char* title, ...);
bool yesnov(const char* title, const char* title_param);