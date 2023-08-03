#include "probfd/bisimulation/engine_interfaces.h"

namespace probfd {
namespace bisimulation {

InducedQuotientEvaluator::InducedQuotientEvaluator(
    bisimulation::BisimilarStateSpace* bisim,
    Interval bound,
    value_t def)
    : bisim_(bisim)
    , bound_(bound)
    , default_(def)
{
}

EvaluationResult InducedQuotientEvaluator::evaluate(QuotientState s) const
{
    if (bisim_->is_dead_end(s)) {
        return EvaluationResult(true, bound_.upper);
    }
    if (bisim_->is_goal_state(s)) {
        return EvaluationResult(false, bound_.lower);
    }
    return EvaluationResult(false, default_);
}

} // namespace bisimulation

} // namespace probfd