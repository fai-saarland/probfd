#ifndef PROBFD_HEURISTICS_PDBS_PDB_COMBINATOR_H
#define PROBFD_HEURISTICS_PDBS_PDB_COMBINATOR_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCombinator {
public:
    virtual ~PDBCombinator() = default;

    virtual value_t evaluate(
        const PPDBCollection& database,
        const State& state,
        value_t termination_cost) = 0;

    virtual void print_statistics(utils::LogProxy log) const = 0;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COMBINATOR_H
