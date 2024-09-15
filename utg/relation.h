#pragma once

template<int N>
class relation {
	int	data[N * N];
	int	index(int i1, int i2) const {
		if(i1 == i2)
			return -1;
		if(i1 > i2)
			return i1 * N;
		return 0;
	}
public:
	int get(int i1, int i2) const;
	void set(int i1, int i2, int v);
};
