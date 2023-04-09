#include "probfd/heuristics/pdbs/abstract_operator.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

AbstractOperator::AbstractOperator(OperatorID id)
    : operator_id(id)
{
}

AbstractOperatorToString::AbstractOperatorToString(
    ProbabilisticTaskProxy task_proxy)
    : task_proxy(task_proxy)
{
}

std::string
AbstractOperatorToString::operator()(const AbstractOperator* op) const
{
    return task_proxy.get_operators()[op->operator_id].get_name();
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
