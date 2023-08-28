#include "probfd/heuristics/pdbs/cegar/projection_factory.h"

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"

#include "downward/utils/countdown_timer.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

ProjectionFactory::ProjectionFactory(int goal)
    : goal(goal)
{
}

ProjectionInfo ProjectionFactory::create_projection(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    utils::CountdownTimer& timer) const
{
    // Start with an initial abstraction mapping, projection state space and
    // a value table of appropriate size filled with zeroes.
    std::unique_ptr<StateRankingFunction> abstraction_mapping(
        new StateRankingFunction(task_proxy.get_variables(), {goal}));

    std::unique_ptr<ProjectionStateSpace> projection_mdp(
        new ProjectionStateSpace(
            task_proxy,
            *abstraction_mapping,
            task_cost_function,
            false,
            timer.get_remaining_time()));

    std::unique_ptr<IncrementalValueTableEvaluator> heuristic(
        new IncrementalValueTableEvaluator(abstraction_mapping->num_states()));

    return ProjectionInfo(
        std::move(abstraction_mapping),
        std::move(projection_mdp),
        std::move(heuristic));
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd
