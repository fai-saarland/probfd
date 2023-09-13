#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H

#include "probfd/heuristics/pdbs/evaluators.h"
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

class ProbabilityAwarePatternDatabase;

namespace cegar {

class PolicyExplorationStrategy;

struct CEGARResult {
    ProjectionCollection projections;
    PPDBCollection pdbs;

    ~CEGARResult();
};

class CEGAR {
    class PDBInfo;

    // Random number generator
    const std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Flaw finding strategy
    const std::shared_ptr<PolicyExplorationStrategy> flaw_strategy;

    // behavior defining parameters
    const bool wildcard;
    const int max_pdb_size;
    const int max_collection_size;

    const std::vector<int> goals;
    std::unordered_set<int> blacklisted_variables;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<PDBInfo> pdb_infos;
    PDBInfo* unsolved_end;

    // Takes a variable as key and returns the solutions-entry whose pattern
    // contains said variable. Used for checking if a variable is already
    // included in some pattern as well as for quickly finding the other partner
    // for merging.
    std::unordered_map<int, std::reference_wrapper<PDBInfo>> variable_to_info;

public:
    CEGAR(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        std::shared_ptr<PolicyExplorationStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        int max_collection_size,
        std::vector<int> goals,
        std::unordered_set<int> blacklisted_variables = {});

    ~CEGAR();

    CEGARResult generate_pdbs(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

private:
    void generate_trivial_solution_collection(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        int& collection_size,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

    // If a concrete solution was found, returns a pointer to the corresponding
    // info struct, otherwise nullptr.
    PDBInfo* get_flaws(
        ProbabilisticTaskProxy task_proxy,
        std::vector<Flaw>& flaws,
        std::vector<int>& flaw_offsets,
        value_t termination_cost,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

    bool can_add_variable(
        VariableProxy variable,
        const PDBInfo& info,
        int collection_size) const;
    bool can_merge_patterns(
        const PDBInfo& left,
        const PDBInfo& right,
        int collection_size) const;

    void add_variable_to_pattern(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        PDBInfo& info,
        int var,
        int& collection_size,
        utils::CountdownTimer& timer);
    void merge_patterns(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        PDBInfo& left,
        PDBInfo& right,
        int& collection_size,
        utils::CountdownTimer& timer);

    void refine(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const std::vector<Flaw>& flaws,
        const std::vector<int>& flaw_offsets,
        int& collection_size,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

    void print_collection(utils::LogProxy log) const;

    void reindex_variables(PDBInfo& from, PDBInfo& to);
    void move_info(PDBInfo& from, PDBInfo& to);
    void swap_infos(PDBInfo& from, PDBInfo& to);
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
