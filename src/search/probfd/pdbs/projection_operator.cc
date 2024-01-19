#include "probfd/pdbs/projection_operator.h"

namespace probfd::pdbs {

ProjectionOperator::ProjectionOperator(OperatorID id)
    : operator_id(id)
{
}

ProjectionOperatorToString::ProjectionOperatorToString(
    ProbabilisticTaskProxy task_proxy)
    : task_proxy_(task_proxy)
{
}

std::string
ProjectionOperatorToString::operator()(const ProjectionOperator* op) const
{
    return task_proxy_.get_operators()[op->operator_id].get_name();
}

} // namespace probfd::pdbs
