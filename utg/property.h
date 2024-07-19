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
		constexpr explicit operator bool() const { return type != -1; }
		void		clear() { type = -1; }
	};
	template<typename T>
	struct value : key {
		T			value;
	};
	const char*		id;
	int				type;
	static const char* read(const char* p, int object);
};

template<typename T> void setproperty(int object, int type, T value);

void addnumber(int object, int type, int value);
void removenumber(int object, int type);
void property_initialize();

int getnumber(int object, int type);
int property_add(const char* id, int type);

const char* getstring(int object, int type);
