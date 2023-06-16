#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H

#include "probfd/heuristics/pdbs/pattern_generator.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_policy.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"


#include "probfd/task_proxy.h"

#include "utils/logging.h"
#include "utils/rng.h"

#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

namespace plugins {
class Feature;
}

namespace utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;

namespace cegar {

class FlawFindingStrategy;
class BFSFlawFinder;
class PUCSFlawFinder;
class SamplingFlawFinder;

struct Flaw {
    int solution_index;
    int variable;
    bool is_precondition;
};

/*
 * Implementation notes: The state space needs to be kept to find flaws in the
 * policy. Since it exists anyway, the algorithm is also a producer of
 * projection state spaces, not only of PDBs. Hence the heap allocation to
 * return it to the user, in case he needs it, e.g. to compute the saturated
 * costs of the PDB.
 */
class PDBInfo {
    std::unique_ptr<ProjectionStateSpace> state_space;
    InducedProjectionCostFunction cost_function;
    StateRank initial_state;
    std::unique_ptr<ProbabilityAwarePatternDatabase> pdb;
    std::unique_ptr<ProjectionPolicy> policy;
    bool solved = false;

public:
    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        TaskCostFunction& task_cost_function,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard,
        utils::CountdownTimer& timer);

    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        TaskCostFunction& task_cost_function,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const ProbabilityAwarePatternDatabase& previous,
        int add_var,
        bool wildcard,
        utils::CountdownTimer& timer);

    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        TaskCostFunction& task_cost_function,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const ProbabilityAwarePatternDatabase& merge_left,
        const ProbabilityAwarePatternDatabase& merge_right,
        bool wildcard,
        utils::CountdownTimer& timer);

    const Pattern& get_pattern() const;

    const ProbabilityAwarePatternDatabase& get_pdb() const;

    const ProjectionPolicy& get_policy() const;

    value_t get_policy_cost(const State& state) const;

    std::unique_ptr<ProjectionStateSpace> extract_state_space();
    std::unique_ptr<ProbabilityAwarePatternDatabase> extract_pdb();

    bool is_solved() const;

    void mark_as_solved();

    bool solution_exists() const;

    bool is_goal(StateRank rank) const;
};

class CEGAR {
    friend class cegar::BFSFlawFinder;
    friend class cegar::PUCSFlawFinder;
    friend class cegar::SamplingFlawFinder;

    mutable utils::LogProxy log;

    // Random number generator
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Flaw finding strategy
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy;

    // behavior defining parameters
    const bool wildcard;
    const int max_pdb_size;
    const int max_collection_size;
    const double max_time;

    const std::vector<int> goals;
    std::unordered_set<int> blacklisted_variables;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<std::unique_ptr<PDBInfo>> pdb_infos;

    // Takes a variable as key and returns the index of the solutions-entry
    // whose pattern contains said variable. Used for checking if a variable
    // is already included in some pattern as well as for quickly finding
    // the other partner for merging.
    std::unordered_map<int, int> variable_to_collection_index;

    int collection_size = 0;

public:
    CEGAR(
        const utils::LogProxy& log,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        int max_collection_size,
        double max_time,
        std::vector<int> goals,
        std::unordered_set<int> blacklisted_variables = {});

    std::pair<
        std::unique_ptr<ProjectionCollection>,
        std::unique_ptr<PPDBCollection>>
    generate_pdbs(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function);

protected:
    bool collect_flaws(
        auto facts,
        const State& state,
        int solution_index,
        bool is_precondition,
        std::vector<Flaw>& flaw_list) const
    {
        bool flaws_found = false;

        // Collect all non-satisfied goal fact variables.
        for (const FactProxy fact : facts) {
            const auto& [var, val] = fact.get_pair();

            if (state[var].get_value() != val &&
                !blacklisted_variables.contains(var)) {
                flaws_found = true;
                flaw_list.emplace_back(solution_index, var, is_precondition);
            }
        }

        return flaws_found;
    }

private:
    void generate_trivial_solution_collection(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        utils::CountdownTimer& timer);

    int get_flaws(
        const ProbabilisticTaskProxy& task_proxy,
        std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer);

    bool
    can_add_singleton_pattern(const VariablesProxy& variables, int var) const;
    bool can_add_variable_to_pattern(
        const VariablesProxy& variables,
        int index,
        int var) const;
    bool can_merge_patterns(int index1, int index2) const;

    void add_pattern_for_var(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        int var,
        utils::CountdownTimer& timer);
    void add_variable_to_pattern(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        int index,
        int var,
        utils::CountdownTimer& timer);
    void merge_patterns(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        int index1,
        int index2,
        utils::CountdownTimer& timer);

    void refine(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        const VariablesProxy& variables,
        const std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer);

    void print_collection() const;
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
