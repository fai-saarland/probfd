#include "probfd/bisimulation/engine_interfaces.h"

namespace probfd {
namespace bisimulation {

InducedQuotientEvaluator::InducedQuotientEvaluator(
    BisimilarStateSpace* bisim,
    value_t upper_bound)
    : bisim_(bisim)
    , upper_bound_(upper_bound)
{
}

EvaluationResult InducedQuotientEvaluator::evaluate(QuotientState s) const
{
    if (bisim_->is_dead_end(s)) {
        return EvaluationResult(true, upper_bound_);
    }

    return EvaluationResult(false, 0_vt);
}

} // namespace bisimulation

} // namespace probfd