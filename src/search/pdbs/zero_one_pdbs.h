#ifndef PDBS_ZERO_ONE_PDBS_H
#define PDBS_ZERO_ONE_PDBS_H

#include "types.h"
#include "../operator_cost.h"

class GlobalState;

namespace pdbs {
class ZeroOnePDBs {
    PDBCollection pattern_databases;
public:
    ZeroOnePDBs(OperatorCost cost_type, const PatternCollection &patterns);
    ~ZeroOnePDBs() = default;

    int get_value(const GlobalState &state) const;
    /*
      Returns the sum of all mean finite h-values of every PDB.
      This is an approximation of the real mean finite h-value of the Heuristic,
      because dead-ends are ignored for the computation of the mean finite
      h-values for a PDB. As a consequence, if different PDBs have different
      states which are dead-end, we do not calculate the real mean h-value for
      these states.
    */
    double compute_approx_mean_finite_h() const;
    void dump() const;
};
}

#endif
