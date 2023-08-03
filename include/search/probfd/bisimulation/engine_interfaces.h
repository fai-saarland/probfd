#ifndef PROBFD_BISIMULATION_ENGINE_INTERFACES_H
#define PROBFD_BISIMULATION_ENGINE_INTERFACES_H

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/evaluator.h"
#include "probfd/interval.h"

namespace probfd {
namespace bisimulation {

/// Type alias for evaluators for probabilistic bisimulation quotients.
using QuotientEvaluator = Evaluator<QuotientState>;

struct InducedQuotientEvaluator : public QuotientEvaluator {
    BisimilarStateSpace* bisim_;
    const Interval bound_;
    const value_t default_;

    explicit InducedQuotientEvaluator(
        BisimilarStateSpace* bisim,
        Interval bound,
        value_t default_value = 0);

    EvaluationResult evaluate(QuotientState state) const override;
};

} // namespace bisimulation
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__