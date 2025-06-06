#ifndef PROBFD_PDBS_CEGAR_CEGAR_H
#define PROBFD_PDBS_CEGAR_CEGAR_H

#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/value_type.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Forward Declarations
namespace downward {
class VariablesProxy;
}

namespace downward::utils {
class CountdownTimer;
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::heuristics {
template <typename State>
class BlindEvaluator;
}

namespace probfd::pdbs::cegar {
struct Flaw;
class FlawFindingStrategy;
} // namespace probfd::pdbs::cegar

namespace probfd::pdbs::cegar {

class CEGAR {
    class PDBInfo;

    // Convergence check epsilon
    const value_t convergence_epsilon;

    // Random number generator
    const std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

    // Flaw finding strategy
    const std::shared_ptr<FlawFindingStrategy> flaw_strategy_;

    // behavior defining parameters
    const bool wildcard_;
    const int max_pdb_size_;
    const int max_collection_size_;

    const std::vector<int> goals_;
    std::unordered_set<int> blacklisted_variables_;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<PDBInfo> pdb_infos_;
    std::vector<PDBInfo>::iterator unsolved_end;
    std::vector<PDBInfo>::iterator solved_end;

    // Takes a variable as key and returns an iterator to the solutions-entry
    // whose pattern contains said variable. Used for checking if a variable
    // is already included in some pattern as well as for quickly finding
    // the other partner for merging.
    std::unordered_map<int, std::vector<PDBInfo>::iterator> variable_to_info_;

    int remaining_size_ = max_collection_size_;

public:
    CEGAR(
        value_t convergence_epsilon,
        const std::shared_ptr<downward::utils::RandomNumberGenerator>& rng,
        std::shared_ptr<FlawFindingStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        int max_collection_size,
        std::vector<int> goals,
        std::unordered_set<int> blacklisted_variables = {});

    ~CEGAR();

    void generate_pdbs(
        ProbabilisticTaskProxy task_proxy,
        const std::shared_ptr<FDRSimpleCostFunction>& task_cost_function,
        ProjectionCollection& projections,
        PPDBCollection& pdbs,
        double max_time,
        downward::utils::LogProxy log);

private:
    void generate_trivial_solution_collection(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        downward::utils::CountdownTimer& timer,
        downward::utils::LogProxy log);

    std::vector<PDBInfo>::iterator get_flaws(
        ProbabilisticTaskProxy task_proxy,
        std::vector<Flaw>& flaws,
        std::vector<int>& flaw_offsets,
        downward::utils::CountdownTimer& timer,
        downward::utils::LogProxy log);

    bool can_add_variable_to_pattern(
        const downward::VariablesProxy& variables,
        std::vector<PDBInfo>::iterator info_it,
        int var) const;

    bool can_merge_patterns(
        std::vector<PDBInfo>::iterator info_it1,
        std::vector<PDBInfo>::iterator info_it2) const;

    void add_pattern_for_var(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        const heuristics::BlindEvaluator<StateRank>& h,
        int var,
        downward::utils::CountdownTimer& timer);

    void add_variable_to_pattern(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        std::vector<PDBInfo>::iterator info_it,
        int var,
        downward::utils::CountdownTimer& timer);

    void merge_patterns(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        std::vector<PDBInfo>::iterator info_it1,
        std::vector<PDBInfo>::iterator info_it2,
        downward::utils::CountdownTimer& timer);

    void refine(
        ProbabilisticTaskProxy task_proxy,
        const std::shared_ptr<FDRSimpleCostFunction>& task_cost_function,
        const std::vector<Flaw>& flaws,
        const std::vector<int>& flaw_offsets,
        downward::utils::CountdownTimer& timer,
        downward::utils::LogProxy log);

    void print_collection(downward::utils::LogProxy log) const;
};

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_CEGAR_H
