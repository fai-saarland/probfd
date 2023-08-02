#include "probfd/bisimulation/engine_interfaces.h"

namespace probfd {
namespace bisimulation {

DefaultQuotientEvaluator::DefaultQuotientEvaluator(
    bisimulation::BisimilarStateSpace* bisim,
    Interval bound,
    value_t def)
    : bisim_(bisim)
    , bound_(bound)
    , default_(def)
{
}

EvaluationResult
DefaultQuotientEvaluator::evaluate(bisimulation::QuotientState s) const
{
    if (bisim_->is_dead_end(s)) {
        return EvaluationResult(true, bound_.upper);
    }
    if (bisim_->is_goal_state(s)) {
        return EvaluationResult(false, bound_.lower);
    }
    return EvaluationResult(false, default_);
}

DefaultQuotientCostFunction::DefaultQuotientCostFunction(
    bisimulation::BisimilarStateSpace* bisim,
    Interval bound,
    value_t def)
    : bisim_(bisim)
    , bound_(bound)
    , default_(def)
{
}

TerminationInfo
DefaultQuotientCostFunction::get_termination_info(bisimulation::QuotientState s)
{
    if (bisim_->is_dead_end(s)) {
        return TerminationInfo(false, bound_.upper);
    }
    if (bisim_->is_goal_state(s)) {
        return TerminationInfo(true, bound_.lower);
    }
    return TerminationInfo(false, default_);
}

value_t
DefaultQuotientCostFunction::get_action_cost(bisimulation::QuotientAction)
{
    return 0;
}

} // namespace bisimulation

} // namespace probfd