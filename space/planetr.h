#pragma once

enum planetr_s : unsigned char {
	Food, Industry, Science, Money, Population
};
struct planetr {
	char resources[Population + 1];
	int	get(planetr_s i) const { return resources[i]; }
	void set(planetr_s i, char v) { resources[i] = v; }
};
struct planetri {
	const char* id;
};
