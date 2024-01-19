#ifndef PROBFD_BISIMULATION_EVALUATORS_H
#define PROBFD_BISIMULATION_EVALUATORS_H

#include "probfd/bisimulation/types.h"

#include "probfd/evaluator.h"
#include "probfd/value_type.h"

// Forward Declarations
namespace probfd::bisimulation {
class BisimilarStateSpace;
}

namespace probfd::bisimulation {

/// Type alias for evaluators for probabilistic bisimulation quotients.
using QuotientEvaluator = Evaluator<QuotientState>;

class InducedQuotientEvaluator : public QuotientEvaluator {
    BisimilarStateSpace* bisim_;
    const value_t upper_bound_;

public:
    explicit InducedQuotientEvaluator(
        BisimilarStateSpace* bisim,
        value_t upper_bound = 0);

    [[nodiscard]]
    value_t evaluate(QuotientState state) const override;
};

} // namespace probfd::bisimulation

#endif // PROBFD_BISIMULATION_EVALUATORS_H
