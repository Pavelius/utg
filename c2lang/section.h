#pragma once

struct sectioni {
	int		ids;
	void*	data;
	size_t	size, size_maximum;
	void	add(const void* source, size_t size);
	template<class T> void add(const T& v) { add(&v, sizeof(v)); }
	bool	isvirtual() const { return size_maximum == -1; }
	size_t	ptr() const { return size; }
	void	reserve(size_t size);
	void	setvirtual() { size_maximum = -1; }
};