#pragma once

#include "pattern_collection_information.h"
#include "../../types.h"
#include "../../../../../utils/printable.h"

#include "../../../../../operator_cost.h"

#include <memory>

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) = 0;

    virtual std::shared_ptr<utils::Printable> get_report() const {
        return nullptr;
    };
};

}
}
}