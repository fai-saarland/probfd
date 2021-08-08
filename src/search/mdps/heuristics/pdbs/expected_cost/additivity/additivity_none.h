#include "additivity_strategy.h"

namespace probabilistic {
namespace pdbs {
namespace expected_cost {
namespace additivity {

class AdditivityNone : public AdditivityStrategy {
public:
    AdditivityNone() = default;
    ~AdditivityNone() override = default;

    std::shared_ptr<std::vector<PatternClique>>
    compute_additive_subcollections(const PatternCollection&) override;
};

} // namespace additivity
} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic