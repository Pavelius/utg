#pragma once

class counters {
	static const unsigned maxcount = 5;
	struct element {
		const char*		name;
		char			value;
	};
	element				elements[maxcount];
public:
	constexpr element& operator[](unsigned n) { return elements[n]; }
	void				add(int v) { add(v, 1); }
	void				add(int v, int i) { set(v, get(v) + i); }
	const element*		begin() const { return elements; }
	void				clear();
	const element*		end() const { return elements + maxcount; }
	int					get(int v) const { return elements[v].value; }
	int					getcount() const { return maxcount; }
	const char*			getname(int v) const { return elements[v].name; }
	element*			getpointer(int v) { return elements + v; }
	void				set(int v, int i);
	void				setname(int v, const char* name) { elements[v].name = name; }
};
