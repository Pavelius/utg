#include "main.h"

int planeti::get(indicator_s v) const {
	switch(v) {
	case Resources: return resources;
	case Influence: return influences;
	default: return 0;
	}
}