#ifndef PROBFD_HEURISTICS_PDBS_ADDITIVE_COMBINATOR_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_ADDITIVE_COMBINATOR_FACTORY_H

#include "probfd/heuristics/pdbs/pdb_combinator_factory.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class AdditiveCombinatorFactory : public PDBCombinatorFactory {
public:
    std::unique_ptr<PDBCombinator>
    create_pdb_combinator(ProbabilisticTaskProxy task_proxy, PPDBCollection&)
        override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_ADDITIVE_COMBINATOR_FACTORY_H
