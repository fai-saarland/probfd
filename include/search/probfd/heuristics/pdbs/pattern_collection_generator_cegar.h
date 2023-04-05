#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PATTERN_COLLECTION_GENERATOR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PATTERN_COLLECTION_GENERATOR_CEGAR_H

#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/pattern_generator.h"
#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include "options/options.h"

#include "utils/logging.h"
#include "utils/rng.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace options {
class OptionParser;
}

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;

struct Flaw {
    int solution_index;
    int variable;
};

template <typename PDBType>
class PDBInfo {
    // The state space needs to be kept because it contains the operators and
    // deleting it invalidates the returned policy actions
    ProjectionStateSpace state_space;
    std::unique_ptr<PDBType> pdb;
    std::unique_ptr<AbstractPolicy> policy;
    bool solved = false;

public:
    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard);

    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const PDBType& previous,
        int add_var,
        bool wildcard);

    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const PDBType& merge_left,
        const PDBType& merge_right,
        bool wildcard);

    const Pattern& get_pattern() const;

    const PDBType& get_pdb() const;

    std::unique_ptr<PDBType> steal_pdb();

    const AbstractPolicy& get_policy() const;

    value_t get_policy_cost(const State& state) const;

    bool is_solved() const;

    void mark_as_solved();

    bool solution_exists() const;

    bool is_goal(StateRank rank) const;
};

namespace cegar {
template <typename PDBType>
class FlawFindingStrategy;
template <typename>
class BFSFlawFinder;
template <typename>
class PUCSFlawFinder;
template <typename>
class SamplingFlawFinder;
} // namespace cegar

enum class InitialCollectionType { GIVEN_GOAL, RANDOM_GOAL, ALL_GOALS };

template <typename PDBType>
class PatternCollectionGeneratorCegar
    : public PatternCollectionGenerator<PDBType> {
    template <typename>
    friend class cegar::BFSFlawFinder;
    template <typename>
    friend class cegar::PUCSFlawFinder;
    template <typename>
    friend class cegar::SamplingFlawFinder;

    // Random number generator
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Subcollection finder
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory;

    // Flaw finding strategy
    std::shared_ptr<cegar::FlawFindingStrategy<PDBType>> flaw_strategy;

    // behavior defining parameters
    const bool wildcard;

    const int max_pdb_size;
    const int max_collection_size;

    // set this to true if you want to
    // generate only one pattern
    const bool ignore_goal_violations;

    const int blacklist_size;
    const InitialCollectionType initial;
    const int given_goal;
    const utils::Verbosity verbosity;
    const double max_time;

    std::vector<int> remaining_goals;
    std::unordered_set<int> blacklisted_variables;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<std::unique_ptr<PDBInfo<PDBType>>> pdb_infos;

    // Takes a variable as key and returns the index of the solutions-entry
    // whose pattern contains said variable. Used for checking if a variable
    // is already included in some pattern as well as for quickly finding
    // the other partner for merging.
    std::unordered_map<int, int> variable_to_collection_index;

    int collection_size;

public:
    explicit PatternCollectionGeneratorCegar(const options::Options& opts);

    PatternCollectionGeneratorCegar(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        std::shared_ptr<SubCollectionFinderFactory>
            subcollection_finder_factory,
        std::shared_ptr<cegar::FlawFindingStrategy<PDBType>> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        int max_collection_size,
        bool ignore_goal_violations,
        int blacklisted_variables_size,
        InitialCollectionType arg_initial,
        int given_goal,
        utils::Verbosity verbosity,
        double arg_max_time);

    virtual ~PatternCollectionGeneratorCegar() = default;

    PatternCollectionInformation<PDBType>
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

private:
    void generate_trivial_solution_collection(
        const ProbabilisticTaskProxy& task_proxy);

    bool time_limit_reached(const utils::CountdownTimer& timer) const;

    int get_flaws(
        const ProbabilisticTaskProxy& task_proxy,
        std::vector<Flaw>& flaws);

    bool
    can_add_singleton_pattern(const VariablesProxy& variables, int var) const;
    bool can_add_variable_to_pattern(
        const VariablesProxy& variables,
        int index,
        int var) const;
    bool can_merge_patterns(int index1, int index2) const;

    void add_pattern_for_var(const ProbabilisticTaskProxy& task_proxy, int var);
    void add_variable_to_pattern(
        const ProbabilisticTaskProxy& task_proxy,
        int index,
        int var);
    void merge_patterns(
        const ProbabilisticTaskProxy& task_proxy,
        int index1,
        int index2);

    void refine(
        const ProbabilisticTaskProxy& task_proxy,
        const VariablesProxy& variables,
        const std::vector<Flaw>& flaws);

    void print_collection() const;
};

template <typename PDBType>
void add_pattern_collection_generator_cegar_options_to_parser(
    options::OptionParser& parser);

using ExpCostPatternCollectionGeneratorCegar =
    PatternCollectionGeneratorCegar<SSPPatternDatabase>;
using MaxProbPatternCollectionGeneratorCegar =
    PatternCollectionGeneratorCegar<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
