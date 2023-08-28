#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PROJECTION_INFO_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PROJECTION_INFO_H

#include "probfd/heuristics/pdbs/types.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

class StateRankingFunction;
class ProjectionStateSpace;
class IncrementalValueTableEvaluator;

namespace cegar {

struct ProjectionInfo {
    std::unique_ptr<StateRankingFunction> abstraction_mapping;
    std::unique_ptr<ProjectionStateSpace> projection_mdp;
    std::unique_ptr<IncrementalValueTableEvaluator> heuristic;

    ProjectionInfo(
        std::unique_ptr<StateRankingFunction>&& abstraction_mapping,
        std::unique_ptr<ProjectionStateSpace>&& projection_mdp,
        std::unique_ptr<IncrementalValueTableEvaluator>&& heuristic);

    ProjectionInfo(ProjectionInfo&&) noexcept;
    ProjectionInfo& operator=(ProjectionInfo&&) noexcept;

    ~ProjectionInfo();

    const Pattern& get_pattern() const;
    void on_refinement(int var);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif