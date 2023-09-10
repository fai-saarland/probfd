#ifndef PROBFD_HEURISTICS_PDBS_ADDITIVE_COMBINATOR_H
#define PROBFD_HEURISTICS_PDBS_ADDITIVE_COMBINATOR_H

#include "probfd/heuristics/pdbs/pdb_combinator.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class AdditiveCombinator : public PDBCombinator {
public:
    value_t evaluate(
        const PPDBCollection& database,
        const State& state,
        value_t termination_cost) override;

    virtual void print_statistics(utils::LogProxy log) const override {}
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_ADDITIVE_COMBINATOR_H
