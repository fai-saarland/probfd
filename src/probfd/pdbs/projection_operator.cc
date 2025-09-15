#include "probfd/pdbs/projection_operator.h"

#include "probfd/probabilistic_operator_space.h"

namespace probfd::pdbs {

ProjectionOperatorToString::ProjectionOperatorToString(
    const ProbabilisticOperatorSpace& operators)
    : operators_(operators)
{
}

std::string
ProjectionOperatorToString::operator()(const ProjectionOperator* op) const
{
    return operators_[op->operator_id].get_name();
}

} // namespace probfd::pdbs
