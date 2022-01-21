#pragma once

struct front {
	const char*		id;
	const char*		present;
	short			current, maximum;
	constexpr explicit operator bool() const { return current != 0; }
};
