#ifndef PDBS_PATTERN_GENERATOR_H
#define PDBS_PATTERN_GENERATOR_H

#include "pdbs/pattern_collection_information.h"
#include "pdbs/pattern_information.h"
#include "pdbs/types.h"

#include "utils/printable.h"

#include "operator_cost.h"

#include <memory>

namespace pdbs {
class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) = 0;

    virtual std::shared_ptr<utils::Printable> get_report() const
    {
        return nullptr;
    }
};

class PatternGenerator {
public:
    virtual ~PatternGenerator() = default;

    virtual PatternInformation generate(OperatorCost cost_type) = 0;
};
}

#endif
