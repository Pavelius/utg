#pragma once

struct kering {
	union {
		struct {
			unsigned short c1, c2;
		};
		unsigned		u;
	};
	int					offset;
	static int			compare(const void* v1, const void* v2);
};
