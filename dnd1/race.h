#pragma once

#include "nameable.h"
#include "flagable.h"

typedef flagable<4> raceable;

struct racei : nameable {
	raceable		hate;
}; 
