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
class VariablesProxy;

namespace plugins {
class Feature;
}

namespace utils {
class CountdownTimer;
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs::cegar {
struct Flaw;
class FlawFindingStrategy;
} // namespace probfd::pdbs::cegar

namespace probfd::pdbs::cegar {

struct CEGARResult {
    std::unique_ptr<ProjectionCollection> projections;
    std::unique_ptr<PPDBCollection> pdbs;

    ~CEGARResult();
};

class CEGAR {
    class PDBInfo;

    // Random number generator
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;

    // Flaw finding strategy
    const std::shared_ptr<FlawFindingStrategy> flaw_strategy_;

    // behavior defining parameters
    const bool wildcard_;
    const int max_pdb_size_;
    const int max_collection_size_;
    const double max_time_;

    const std::vector<int> goals_;
    std::unordered_set<int> blacklisted_variables_;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<std::unique_ptr<PDBInfo>> pdb_infos_;

    // Takes a variable as key and returns the index of the solutions-entry
    // whose pattern contains said variable. Used for checking if a variable
    // is already included in some pattern as well as for quickly finding
    // the other partner for merging.
    std::unordered_map<int, int> variable_to_collection_index_;

    int collection_size_ = 0;

public:
    CEGAR(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        int max_collection_size,
        double max_time,
        std::vector<int> goals,
        std::unordered_set<int> blacklisted_variables = {});

    ~CEGAR();

    CEGARResult generate_pdbs(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        utils::LogProxy log);

private:
    void generate_trivial_solution_collection(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        utils::CountdownTimer& timer,
        utils::LogProxy log);

    int get_flaws(
        ProbabilisticTaskProxy task_proxy,
        std::vector<Flaw>& flaws,
        std::vector<int>& flaw_offsets,
        utils::CountdownTimer& timer,
        utils::LogProxy log);

    bool
    can_add_singleton_pattern(const VariablesProxy& variables, int var) const;

    bool can_add_variable_to_pattern(
        const VariablesProxy& variables,
        int index,
        int var) const;

    bool can_merge_patterns(int index1, int index2) const;

    void add_pattern_for_var(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        int var,
        utils::CountdownTimer& timer);

    void add_variable_to_pattern(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        int index,
        int var,
        utils::CountdownTimer& timer);

    void merge_patterns(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        int index1,
        int index2,
        utils::CountdownTimer& timer);

    void refine(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const VariablesProxy& variables,
        const std::vector<Flaw>& flaws,
        const std::vector<int>& flaw_offsets,
        utils::CountdownTimer& timer,
        utils::LogProxy log);

    void print_collection(utils::LogProxy log) const;
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_CEGAR_H
