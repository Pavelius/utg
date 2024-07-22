#pragma once

template<typename T, typename ... TS>
constexpr unsigned fg(T n, ...) {
	return (1 << n);
};