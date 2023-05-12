#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_FINDER_H

#include "probfd/task_proxy.h"

#include <string>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionPolicy;

namespace cegar {

struct Flaw;
class CEGAR;

class FlawFindingStrategy {
public:
    virtual ~FlawFindingStrategy() = default;

    // Returns whether policy is executable (modulo blacklisting)
    // Note that the output flaw list might be empty regardless since only
    // remaining goals are added to the list for goal violations.
    virtual bool apply_policy(
        const CEGAR& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer) = 0;

    virtual std::string get_name() = 0;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif