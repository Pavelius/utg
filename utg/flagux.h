#pragma once

template<unsigned C>
class flagux {
	static constexpr unsigned s = 32;
	static constexpr unsigned N = (C + s - 1) / s;
	unsigned		data[N];
public:
	constexpr explicit operator bool() const { for(auto e : data) if(e) return true; return false; }
	constexpr flagux() : data{0} {}
	void			clear() { memset(this, 0, sizeof(*this)); }
	constexpr bool	is(short unsigned v) const { return (data[v / s] & (1 << (v % s))) != 0; }
	constexpr bool	is(const flagux& e) const { for(unsigned i = 0; i < N; i++) if((data[i] & e.data[i]) != 0) return true; return false; }
	constexpr int	getcount() const { auto r = 0; for(auto i = 0; i < N * s; i++) if(is(i)) r++; return r; }
	constexpr int	getmaximum() const { return N * s; }
	constexpr void	remove(short unsigned v) { data[v / s] &= ~(1 << (v % s)); }
	constexpr void	set(short unsigned v) { data[v / s] |= 1 << (v % s); }
	constexpr void	set(short unsigned v, bool activate) { if(activate) set(v); else remove(v); }
};
