#pragma once

#include "../../types.h"

#include <memory>
#include <vector>

namespace probabilistic {
namespace pdbs {
namespace expected_cost {
namespace additivity {

class AdditivityStrategy {
public:
    virtual ~AdditivityStrategy() = default;

    virtual std::shared_ptr<std::vector<PatternClique>>
    compute_additive_subcollections(const PatternCollection&) = 0;
};

} // namespace additivity
} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic