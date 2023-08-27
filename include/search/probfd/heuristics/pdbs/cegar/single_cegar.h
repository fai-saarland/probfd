#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H

#include "probfd/heuristics/pdbs/algorithm_interfaces.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

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

    mutable utils::LogProxy log;

    // Random number generator
    const std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Flaw finding strategy
    const std::shared_ptr<FlawFindingStrategy> flaw_strategy;

    // behavior defining parameters
    const bool wildcard;
    const int max_pdb_size;
    const double max_time;

    const int goal;

    std::unordered_set<int> blacklisted_variables;

    /// The projection in form of its pdb plus stored policy.
    std::unique_ptr<PDBInfo> pdb_info;

public:
    SingleCEGAR(
        const utils::LogProxy& log,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        double max_time,
        int goal,
        std::unordered_set<int> blacklisted_variables = {});

    ~SingleCEGAR();

    SingleCEGARResult generate_pdb(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function);

private:
    bool get_flaws(
        const ProbabilisticTaskProxy& task_proxy,
        std::vector<Flaw>& flaws,
        value_t termination_cost,
        utils::CountdownTimer& timer);

    bool can_add_variable(const VariablesProxy& variables, int var) const;

    void add_variable_to_pattern(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        int var,
        utils::CountdownTimer& timer);

    void refine(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const VariablesProxy& variables,
        const std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer);
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
