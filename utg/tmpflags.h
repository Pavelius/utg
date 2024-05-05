#pragma once

template<typename T, typename ... TS>
constexpr unsigned tpflg(T n, ...) {
	return (1<<n) | 
};