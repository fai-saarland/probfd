#include "probfd/heuristics/pdbs/abstract_operator.h"

#include "probfd/probabilistic_operator.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

AbstractOperator::AbstractOperator(unsigned id, int reward)
    : original_operator_id(id)
    , reward(reward)
{
}

AbstractOperatorToString::AbstractOperatorToString(
    const std::vector<ProbabilisticOperator>* ops)
    : ops_(ops)
{
}

std::string
AbstractOperatorToString::operator()(const AbstractOperator* op) const
{
    return ops_->operator[](op->original_operator_id).get_name();
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
