#pragma once

class counters {
	static const unsigned maxcount = 8;
	struct element {
		const char		name[16];
		char			value, maximum;
	};
	element				elements[maxcount];
public:
	constexpr element& operator[](unsigned n) { return elements[n]; }
	void				clear();
	const char*			getname(int v) const { return elements[v].name; }
	int					get(int v) const { return elements[v].value; }
	void				set(int v, int i);
	void				setname(int v, const char* name);
};
