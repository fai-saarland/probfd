#pragma once

#include "multiplicativity_strategy.h"

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace multiplicativity {

class MultiplicativityMaxOrthogonality : public MultiplicativityStrategy {
public:
    MultiplicativityMaxOrthogonality() = default;
    ~MultiplicativityMaxOrthogonality() override = default;

    std::shared_ptr<std::vector<PatternClique>>
    compute_multiplicative_subcollections(const PatternCollection&) override;
};

} // namespace multiplicativity
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic