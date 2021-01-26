#include "abstract_operator.h"

#include "../../probabilistic_operator.h"

namespace probabilistic {
namespace pdbs {

AbstractOperator::AbstractOperator(unsigned id)
    : original_operator_id(id)
    , pre(0)
{
}

AbstractOperatorToString::AbstractOperatorToString(
    const std::vector<ProbabilisticOperator>* ops)
    : ops_(ops)
{
}

std::string
AbstractOperatorToString::operator()(const AbstractOperator* op)
{
    return ops_->operator[](op->original_operator_id).get_name();
}

} // namespace pdbs
} // namespace probabilistic
