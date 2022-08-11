#pragma once

template<typename T, unsigned N, typename CT = char>
struct cdat {
	CT		data[N];
	void	add(T i, CT v) { data[i] += v; }
	int		get(T i) const { return data[i]; }
	void	set(T i, CT v) { data[i] = v; }
};
