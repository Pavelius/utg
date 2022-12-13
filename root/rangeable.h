#include "tag.h"

#pragma once

struct rangeable {
	tag_s range = Close;
	void addrange(int v);
	void setrange(tag_s v) { range = v; }
};
