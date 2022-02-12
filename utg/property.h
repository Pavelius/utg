#pragma once

class array;
struct propertyi {
	enum {
		Text, Number,
	};
	struct typei {
		const char*	id;
		array*		source;
		static void	add(const char* id, array* source);
	};
	struct key {
		int			object, type;
		constexpr explicit operator bool() const { return type != 0; }
	};
	template<typename T>
	struct value : key {
		T			value;
	};
	const char*		id;
	int				type;
	static int		add(const char* id, int type);
	static void		initialize();
	static const char* read(const char* p, int object);
};
void addnumber(int object, int type, int value);
template<typename T> void setproperty(int object, int type, T value);
int getnumber(int object, int type);
const char* getstring(int object, int type);
void removenumber(int object, int type);