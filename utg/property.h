#pragma once

class array;
struct propertyi {
	typedef short unsigned indext;
	enum {
		Text, Number,
	};
	struct typei {
		const char*	id;
		array*		source;
		static void	add(const char* id, array* source);
	};
	struct key {
		indext		object, type;
		constexpr explicit operator bool() const { return type != 0; }
	};
	template<typename T>
	struct value : key {
		T			value;
	};
	const char*		id;
	int				type;
	static indext	add(const char* id, int type);
	static void		initialize();
	static const char* read(const char* p, indext object);
};
int getnumber(propertyi::indext object, propertyi::indext type);
const char* getstring(propertyi::indext object, propertyi::indext type);
template<typename T> void addproperty(propertyi::indext object, propertyi::indext type, T value);