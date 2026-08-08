#include "probfd/occupation_measures/union_constraint_generator.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/utils/collections.h"

#include "downward/lp/lp_solver.h"
#include "probfd/probabilistic_operator_space.h"

#include <iterator>
#include <set>
#include <utility>
#include <vector>

using namespace downward;

namespace probfd::occupation_measures {

UnionConstraintGenerator::UnionConstraintGenerator(
    std::vector<std::unique_ptr<ConstraintGenerator>> generators)
    : generators_(std::move(generators))
{
}

void UnionConstraintGenerator::initialize_constraints(
    const SharedProbabilisticTask& task,
    lp::LinearProgram& lp)
{
    for (const auto& generator : generators_) {
        generator->initialize_constraints(task, lp);
    }
}

void UnionConstraintGenerator::update_constraints(
    const State& state,
    lp::LPSolver& solver)
{
    for (const auto& generator : generators_) {
        generator->update_constraints(state, solver);
    }
}

void UnionConstraintGenerator::reset_constraints(
    const State& state,
    lp::LPSolver& solver)
{
    for (const auto& generator : generators_) {
        generator->reset_constraints(state, solver);
    }
}

UnionConstraintGeneratorFactory::UnionConstraintGeneratorFactory(
    std::vector<std::shared_ptr<ConstraintGeneratorFactory>>
        generators_factories)
    : generators_factories_(std::move(generators_factories))
{
}

std::unique_ptr<ConstraintGenerator>
UnionConstraintGeneratorFactory::construct_constraint_generator(
    const SharedProbabilisticTask& task)
{
    return std::make_unique<UnionConstraintGenerator>(
        generators_factories_ |
        std::views::transform([&](const auto& generator_factory) {
            return generator_factory->construct_constraint_generator(task);
        }) |
        std::ranges::to<std::vector>());
}

} // namespace probfd::occupation_measures
