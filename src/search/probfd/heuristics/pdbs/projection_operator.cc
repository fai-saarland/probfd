#include "probfd/heuristics/pdbs/projection_operator.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

ProjectionOperator::ProjectionOperator(OperatorID id)
    : operator_id(id)
{
}

ProjectionOperatorToString::ProjectionOperatorToString(
    ProbabilisticTaskProxy task_proxy)
    : task_proxy(task_proxy)
{
}

std::string
ProjectionOperatorToString::operator()(const ProjectionOperator* op) const
{
    return task_proxy.get_operators()[op->operator_id].get_name();
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
