#pragma once

struct reporti {
	const char*	text;
	const char*	header;
	unsigned	turn, reciever;
	static void	add(const char* format, const char* header, unsigned turn, unsigned reciever);
};
