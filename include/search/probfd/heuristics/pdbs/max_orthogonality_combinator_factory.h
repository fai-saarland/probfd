#ifndef PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONALITY_COMBINATOR_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONALITY_COMBINATOR_FACTORY_H

#include "probfd/heuristics/pdbs/incremental_pdb_combinator_factory.h"

namespace plugins {
class Options;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxOrthogonalityCombinatorFactory
    : public IncrementalPDBCombinatorFactory {
    const bool multiplicative;

public:
    explicit MaxOrthogonalityCombinatorFactory(const plugins::Options& opts);
    explicit MaxOrthogonalityCombinatorFactory(bool multiplicative);

    std::unique_ptr<IncrementalPDBCombinator> create_incremental_pdb_combinator(
        ProbabilisticTaskProxy task_proxy) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONALITY_COMBINATOR_FACTORY_H
