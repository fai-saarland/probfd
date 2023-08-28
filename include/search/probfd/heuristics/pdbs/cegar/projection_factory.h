#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_INITIAL_ABSTRACTION_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_INITIAL_ABSTRACTION_FACTORY_H

#include "probfd/heuristics/pdbs/cegar/projection_info.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include <memory>

namespace utils {
class CountdownTimer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class ProjectionFactory {
    const int goal;

public:
    explicit ProjectionFactory(int goal);

    ProjectionInfo create_projection(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        utils::CountdownTimer& timer) const;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif