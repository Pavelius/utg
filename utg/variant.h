#include "crt.h"
#include "stringbuilder.h"

#pragma once

#define VKIND(T, V) template<> constexpr variant_s variant::kind<T>() { return V; }

struct bsreq;
union variant;
enum variant_s : unsigned char;

typedef void (*fngetinfo)(const void* object, variant v, stringbuilder& sb);

struct varianti {
	enum { NotFoundByName, FoundByIndex };
	const char*			id;
	const bsreq*		metadata;
	array*				source;
	int                 key_count;
	unsigned			flags;
	fngetname			pgetname, pgetavatar;
	fnstatus			pgetinfo;
	fngetinfo			pgetproperty;
	static const array* getarray(const void* object, const char* id);
	static const varianti* getsource(const char* id);
	static const varianti* getmetadata(const void* object);
	int					found(const char* id) const;
	constexpr bool		is(unsigned v) const { return (flags & FG(v)) != 0; }
	constexpr bool		isnamed() const { return !is(NotFoundByName); }
	void				set(void* object, const char* id, void* value) const;
	void				set(void* object, const char* id, int value) const;
};
union variant {
	unsigned char		uc[4];
	unsigned			u;
	struct {
		unsigned short	value;
		unsigned char	counter;
		variant_s		type;
	};
	constexpr variant() : u(0) {}
	constexpr variant(variant_s t, unsigned short n) : value(n), counter(0), type(t) {}
	constexpr variant(int u) : u(u) {}
	template<class T> static constexpr variant_s kind();
	template<class T> variant(T* v) : variant((const void*)v) {}
	template<class T> constexpr variant(T v) : variant(kind<T>(), v) {}
	constexpr operator int() const { return u; }
	constexpr explicit operator bool() const { return u != 0; }
	constexpr bool operator==(const variant& v) const { return u == v.u; }
	constexpr bool operator!=(const variant& v) const { return u != v.u; }
	template<class T> operator T*() const { return (T*)((kind<T>() == type) ? getpointer() : 0); }
	void				clear() { u = 0; }
	const char*			getdescription() const;
	const varianti&		geti() const { return bsdata<varianti>::elements[type]; }
	const char*			getid() const;
	int					getindex(int t) const { return (type == t) ? value : 0; }
	void				getinfo(stringbuilder& sb) const;
	void*				getpointer() const { return geti().source->ptr(value); }
	const char*			getname() const;
	void				setvariant(variant_s t, unsigned short v) { type = t; value = v; }
};
typedef sliceu<variant> variants;
template<> variant::variant(const char* v);
template<> variant::variant(const void* v);
