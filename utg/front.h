#pragma once

struct front {
	const char*		id;
	short			current, maximum;
	constexpr explicit operator bool() const { return current != 0; }
};
