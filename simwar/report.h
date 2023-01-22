#pragma once

struct reporti {
	const char*	text;
	unsigned	turn, reciever;
	static void	add(const char* format, unsigned turn, unsigned reciever);
};
