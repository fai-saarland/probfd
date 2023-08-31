#include "probfd/bisimulation/algorithm_interfaces.h"

namespace probfd {
namespace bisimulation {

InducedQuotientEvaluator::InducedQuotientEvaluator(
    BisimilarStateSpace* bisim,
    value_t upper_bound)
    : bisim_(bisim)
    , upper_bound_(upper_bound)
{
}

value_t InducedQuotientEvaluator::evaluate(QuotientState s) const
{
    return bisim_->is_dead_end(s) ? upper_bound_ : 0_vt;
}

} // namespace bisimulation

} // namespace probfd