#include "probfd/pdbs/projection_operator.h"

#include "probfd/probabilistic_task.h"

namespace probfd::pdbs {

ProjectionOperatorToString::ProjectionOperatorToString(
    const ProbabilisticTask& task)
    : task_(task)
{
}

std::string
ProjectionOperatorToString::operator()(const ProjectionOperator* op) const
{
    return task_.get_operators()[op->operator_id].get_name();
}

} // namespace probfd::pdbs
