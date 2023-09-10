#ifndef PROBFD_HEURISTICS_PDBS_MAXIMUM_COMBINATOR_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_MAXIMUM_COMBINATOR_FACTORY_H

#include "probfd/heuristics/pdbs/incremental_pdb_combinator_factory.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaximumCombinatorFactory : public IncrementalPDBCombinatorFactory {
public:
    std::unique_ptr<IncrementalPDBCombinator>
        create_incremental_pdb_combinator(ProbabilisticTaskProxy) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_MAXIMUM_COMBINATOR_FACTORY_H
