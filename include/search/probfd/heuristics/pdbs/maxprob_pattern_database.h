#ifndef PROBFD_HEURISTICS_PDBS_MAXPROB_PATTERN_DATABASE_H
#define PROBFD_HEURISTICS_PDBS_MAXPROB_PATTERN_DATABASE_H

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

/// Implements a probabilistic pattern database for MaxProb.
class MaxProbPatternDatabase : public ProbabilisticPatternDatabase {
public:
    using ProbabilisticPatternDatabase::ProbabilisticPatternDatabase;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __MAXPROB_PROJECTION_H__