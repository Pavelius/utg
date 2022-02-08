#pragma once

const int AnswerGoal = 6300;

class goal {
	short unsigned	value;
	typedef void (*fninfo)(const char* format, ...);
public:
	static void		initialize();
	static fninfo	info;
	int				getgoal() const { return value; }
	const char*		getgoalname() const;
	const char*		getgoaltext() const;
	int				getgoalmaximum() const;
	void			setgoal(int v);
};
