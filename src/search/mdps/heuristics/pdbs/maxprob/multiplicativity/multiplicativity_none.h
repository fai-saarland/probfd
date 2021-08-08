#include "multiplicativity_strategy.h"

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace multiplicativity {

class MultiplicativityNone : public MultiplicativityStrategy {
public:
    MultiplicativityNone() = default;
    ~MultiplicativityNone() override = default;

    std::shared_ptr<std::vector<PatternClique>>
    compute_multiplicative_subcollections(const PatternCollection&) override;
};

} // namespace multiplicativity
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic