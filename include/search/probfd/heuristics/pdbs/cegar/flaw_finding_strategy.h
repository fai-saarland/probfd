#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_FINDER_H

#include "probfd/task_proxy.h"

#include <string>

namespace probfd {
namespace heuristics {
namespace pdbs {

class AbstractPolicy;
struct Flaw;

class PatternCollectionGeneratorCegar;

namespace cegar {

class FlawFindingStrategy {
public:
    virtual ~FlawFindingStrategy() = default;

    // Returns whether policy is executable (modulo blacklisting)
    // Note that the output flaw list might be empty regardless since only
    // remaining goals are added to the list for goal violations.
    virtual bool apply_policy(
        PatternCollectionGeneratorCegar& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaws) = 0;

    virtual std::string get_name() = 0;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif