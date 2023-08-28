#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

class State;

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {

class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {
namespace cegar {

struct Flaw;
class FlawFindingStrategy;
struct ProjectionInfo;

class FlawGenerator {
    // Flaw finding strategy
    const std::shared_ptr<FlawFindingStrategy> flaw_strategy;

    // Random number generator (to pick flaws randomly)
    const std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Find flaws in wildcard policies?
    const bool wildcard;

    // The maximum PDB size. Do not report a flaw if the refinement would
    // increase the PDB size beyond this threshold.
    const int max_pdb_size;

    // Blacklist of variables to ignore when encountered as flaws
    std::unordered_set<int> blacklisted_variables;

    std::vector<Flaw> flaws;

public:
    FlawGenerator(
        std::shared_ptr<FlawFindingStrategy> flaw_strategy,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool wildcard,
        int max_pdb_size,
        std::unordered_set<int> blacklisted_variables = {});

    /**
     * @brief Generates a single flaw for the given projection.
     */
    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        ProjectionInfo& projection_info,
        const State& state,
        value_t termination_cost,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

    std::string get_name() const;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif