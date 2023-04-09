#ifndef PROBFD_HEURISTICS_PDBS_EXPCOST_PROJECTION_H
#define PROBFD_HEURISTICS_PDBS_EXPCOST_PROJECTION_H

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

/// Implements a probabilistic pattern database for SSPs.
class SSPPatternDatabase : public ProbabilisticPatternDatabase {
public:
    using ProbabilisticPatternDatabase::ProbabilisticPatternDatabase;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __EXPCOST_PROJECTION_H__