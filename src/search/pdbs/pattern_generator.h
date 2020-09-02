#ifndef PDBS_PATTERN_GENERATOR_H
#define PDBS_PATTERN_GENERATOR_H

#include "pattern_collection_information.h"
#include "pattern_information.h"
#include "types.h"
#include "../operator_cost.h"

#include <memory>

namespace pdbs {
class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) = 0;
};

class PatternGenerator {
public:
    virtual ~PatternGenerator() = default;

    virtual PatternInformation generate(OperatorCost cost_type) = 0;
};
}

#endif
