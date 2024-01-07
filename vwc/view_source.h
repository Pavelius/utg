#include "color.h"

#pragma once

namespace colors {
namespace source {
extern color	text, number, keyword;
}
}

namespace code {
struct symbol;
extern symbol* active_symbol;

void view_code_source();

}