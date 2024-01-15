#ifndef PROBFD_BISIMULATION_EVALUATORS_H
#define PROBFD_BISIMULATION_EVALUATORS_H

#include "probfd/bisimulation/types.h"

#include "probfd/evaluator.h"
#include "probfd/value_type.h"

namespace probfd {
namespace bisimulation {
class BisimilarStateSpace;

/// Type alias for evaluators for probabilistic bisimulation quotients.
using QuotientEvaluator = Evaluator<QuotientState>;

struct InducedQuotientEvaluator : public QuotientEvaluator {
    BisimilarStateSpace* bisim_;
    const value_t upper_bound_;

    explicit InducedQuotientEvaluator(
        BisimilarStateSpace* bisim,
        value_t upper_bound = 0);

    value_t evaluate(QuotientState state) const override;
};

} // namespace bisimulation
} // namespace probfd

#endif // PROBFD_BISIMULATION_EVALUATORS_H
