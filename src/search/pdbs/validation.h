#ifndef PDBS_VALIDATION_H
#define PDBS_VALIDATION_H

#include "types.h"

namespace pdbs {
extern void validate_and_normalize_pattern(Pattern &pattern);
extern void validate_and_normalize_patterns(PatternCollection &patterns);
}

#endif
