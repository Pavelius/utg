#include "answers.h"

#pragma once

struct choosestep {
	typedef void(*fnanswer)(stringbuilder& sbtitle, answers& an);
	typedef void(*fnaichoose)(answers& an);
	const char*		id;
	fnanswer		panswer;
	fnevent			papply;
	const char*		cancel;
	fnaichoose		paichoose;
	fnevent			pbefore, pafter;
	static bool		applied, human, stop;
	static int		options;
	static void*	result;
	static fnanswer	pstd_answer;
	static fnevent	pstd_apply;
	static void		addscript(answers& an, const char* id);
	void			run() const;
	static void		run(const char* id);
};
extern const choosestep* laststep;