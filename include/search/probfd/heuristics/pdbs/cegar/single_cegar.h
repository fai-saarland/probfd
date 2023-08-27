#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H

#include "probfd/heuristics/pdbs/algorithm_interfaces.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;

namespace cegar {

class FlawFindingStrategy;

struct SingleCEGARResult {
    std::unique_ptr<ProjectionStateSpace> projection;
    std::unique_ptr<ProbabilityAwarePatternDatabase> pdb;

    ~SingleCEGARResult();
};

class SingleCEGAR {
    class PDBInfo;

    // Early termination criteria
    const int max_pdb_size;
    const double max_time;

    // Flaw finding strategy
    const std::shared_ptr<FlawFindingStrategy> flaw_strategy;

    // Random number generator (to pick flaws randomly)
    const std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Find flaws in wildcard policies?
    const bool wildcard;

    // Goal variable for initial projection
    const int goal;

    // Log output
    mutable utils::LogProxy log;

    // Blacklist of variables to ignore when encountered as flaws
    std::unordered_set<int> blacklisted_variables;

public:
    SingleCEGAR(
        int max_pdb_size,
        double max_time,
        std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool wildcard,
        int goal,
        utils::LogProxy log,
        std::unordered_set<int> blacklisted_variables = {});

    ~SingleCEGAR();

    SingleCEGARResult generate_pdb(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function);

private:
    std::optional<Flaw> get_flaw(
        const PDBInfo& pdb_info,
        const State& state,
        const ProbabilisticTaskProxy& task_proxy,
        std::vector<Flaw>& flaws,
        value_t termination_cost,
        utils::CountdownTimer& timer);

    bool can_add_variable(
        const PDBInfo& pdb_info,
        const VariablesProxy& variables,
        int var) const;

    void add_variable_to_pattern(
        PDBInfo& pdb_info,
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        int var,
        utils::CountdownTimer& timer);

    void refine(
        PDBInfo& pdb_info,
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Flaw flaw,
        utils::CountdownTimer& timer);
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
