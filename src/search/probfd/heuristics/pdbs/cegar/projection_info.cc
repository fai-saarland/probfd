#include "probfd/heuristics/pdbs/cegar/projection_info.h"

#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/evaluators.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

ProjectionInfo::ProjectionInfo(
    std::unique_ptr<StateRankingFunction>&& abstraction_mapping,
    std::unique_ptr<ProjectionStateSpace>&& projection_mdp,
    std::unique_ptr<IncrementalValueTableEvaluator>&& heuristic)
    : abstraction_mapping(std::move(abstraction_mapping))
    , projection_mdp(std::move(projection_mdp))
    , heuristic(std::move(heuristic))
{
}

ProjectionInfo::ProjectionInfo(ProjectionInfo&&) noexcept = default;
ProjectionInfo& ProjectionInfo::operator=(ProjectionInfo&&) noexcept = default;
ProjectionInfo::~ProjectionInfo() = default;

const Pattern& ProjectionInfo::get_pattern() const
{
    return abstraction_mapping->get_pattern();
}

void ProjectionInfo::on_refinement(int var)
{
    heuristic->on_refinement(*abstraction_mapping, var);
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd