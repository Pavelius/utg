#include "answers.h"
#include "crt.h"
#include "draw_figure.h"
#include "flagable.h"
#include "front.h"
#include "menu.h"
#include "point.h"
#include "variant.h"

#pragma once

struct widget {
	const char*		id;
	fnevent			proc;
	explicit operator bool() const { return id != 0; }
};
namespace draw {
extern array*		heroes;
extern fngetname	heroes_getavatar;
extern const void*	focus_object;
extern const void*	hilite_object;
extern figure		hilite_type;
extern fnstatus		pstatus;
extern int			title_width;
void				answerbt(int index, const void* id, const char* title);
void				avatar(int index, const void* object, const char* id);
void				avatarch(int index, const void* object, const char* id);
bool				buttonfd(const char* title);
void				label(const char* id, const char* value);
void				label(const char* id, const char* value, const void* object);
void				label(const void* object, const variants& elements, fngetinfo pget);
void				noavatar();
void				texth2(const char* title);
void				vertical(fnevent proc);
namespace utg {
void				beforemodal();
void				paint();
int					run(fnevent proc, bool darkmode, fnevent afterread = 0);
void				tips();
}
}
typedef flagable<4> flaga;
namespace logs {
class choosei {
	answers&		source;
	char			marked[32];
public:
	choosei(answers& e) : source(e), marked() {}
	virtual void	apply(int index, const void* object) { }
	void			addmarked(int index);
	void			choose(const char* title, int count);
	int				getmarked(int v) const;
	int				getindex(const void* row) { return source.indexof(row); }
	virtual bool	isallow(int index, const void* object) const;
	bool			ismarked(int index) const { return getmarked(index) != 0; }
};
extern stringbuilder sb;
extern const char*	url;
extern const char*	url_avatars;
extern const char*	header;
extern bool			interactive;
const char*			chooseavatar(answers& an, const char* title);
void				chooseavatar(stringbuilder& result, const char* title, const char* mask = 0);
void*				choose(const answers& source, const char* title);
int					choose(const answers& source, const char* title, const char* cancel);
int					choose(const array& source, const char* title, const char* cancel);
int					choose(const array& source, const flaga& mask, const char* title, const char* cancel);
const char*			getchoose(const char* id);
void				pause();
void				pause(const char* title);
}