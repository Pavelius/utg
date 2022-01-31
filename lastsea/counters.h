#pragma once

class counters {
	static const unsigned maxcount = 5;
	struct element {
		const char*		name;
		char			value, maximum;
	};
	element				elements[maxcount];
public:
	constexpr element& operator[](unsigned n) { return elements[n]; }
	void				clear();
	void				add(int v) { add(v, 1); }
	void				add(int v, int i) { set(v, get(v) + i); }
	int					get(int v) const { return elements[v].value; }
	const char*			getname(int v) const { return elements[v].name; }
	element*			getpointer(int v) { return elements + v; }
	void				set(int v, int i);
	void				setname(int v, const char* name) { elements[v].name = name; }
};
