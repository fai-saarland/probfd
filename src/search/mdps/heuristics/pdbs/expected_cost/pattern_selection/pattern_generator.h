#pragma once

#include "../../../../../utils/printable.h"
#include "../../types.h"
#include "pattern_collection_information.h"

#include "../../../../../operator_cost.h"

#include <memory>

namespace probabilistic {
namespace pdbs {
namespace expected_cost {

/// Namespace dedicated to pattern selection techniques for probabilistic PDBs
namespace pattern_selection {

class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) = 0;

    virtual std::shared_ptr<utils::Printable> get_report() const
    {
        return nullptr;
    };
};

} // namespace pattern_selection
} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic