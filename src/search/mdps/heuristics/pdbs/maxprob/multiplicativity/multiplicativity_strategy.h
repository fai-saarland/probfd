#pragma once

#include "../../types.h"

#include <memory>
#include <vector>

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace multiplicativity {

class MultiplicativityStrategy {
public:
    virtual ~MultiplicativityStrategy() = default;

    virtual std::shared_ptr<std::vector<PatternClique>>
    compute_multiplicative_subcollections(const PatternCollection&) = 0;
};

} // namespace multiplicativity
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic