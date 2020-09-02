#ifndef PDBS_ZERO_ONE_PDBS_HEURISTIC_H
#define PDBS_ZERO_ONE_PDBS_HEURISTIC_H

#include "zero_one_pdbs.h"

#include "../heuristic.h"

namespace pdbs {
class PatternDatabase;

class ZeroOnePDBsHeuristic : public Heuristic {
    ZeroOnePDBs zero_one_pdbs;
protected:
    virtual int compute_heuristic(const GlobalState &global_state) override;
public:
    ZeroOnePDBsHeuristic(const options::Options &opts);
    virtual ~ZeroOnePDBsHeuristic() = default;
};
}

#endif
