#pragma once

enum rollopt_s : unsigned char {
	HelpDice,
	TraitsHelp, TraitsImpede, TraitsHurt, TraitsBreakTie,
	IAmWise, DeeperUnderstanding, OfCourse,
};
struct rolli {
	rollopt_s	option;
	class hero*	player;
	const void*	action;
};
