#pragma once

enum planetr_s : unsigned char {
	Food, Industry, Science, Money, Population
};
struct planetr {
	char		resources[Population + 1];
};
struct planetri {
	const char* id;
};
