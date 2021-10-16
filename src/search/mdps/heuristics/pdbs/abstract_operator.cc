#include "abstract_operator.h"

#include "../../probabilistic_operator.h"

namespace probabilistic {
namespace pdbs {

AbstractOperator::AbstractOperator(unsigned id, int cost)
    : original_operator_id(id)
    , cost(cost)
{
}

AbstractRegressionOperator::AbstractRegressionOperator(
    unsigned id,
    AbstractState effect)
    : abstract_operator_id(id)
    , effect(effect)
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
} // namespace probabilistic
