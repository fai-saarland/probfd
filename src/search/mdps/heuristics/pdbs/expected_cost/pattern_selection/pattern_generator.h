#pragma once

#include "pattern_collection_information.h"
#include "../../types.h"

#include "../../../../../operator_cost.h"

#include <memory>

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) = 0;
};

}
}
}