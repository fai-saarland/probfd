#include "probfd/bisimulation/evaluators.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

namespace probfd::bisimulation {

InducedQuotientEvaluator::InducedQuotientEvaluator(BisimilarStateSpace* bisim)
    : bisim_(bisim)
{
}

value_t InducedQuotientEvaluator::evaluate(QuotientState s) const
{
    return bisim_->is_dead_end(s) ? INFINITE_VALUE : 0_vt;
}

} // namespace probfd::bisimulation
