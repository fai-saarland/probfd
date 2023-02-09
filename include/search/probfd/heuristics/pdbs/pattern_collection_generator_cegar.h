#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PATTERN_COLLECTION_GENERATOR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PATTERN_COLLECTION_GENERATOR_CEGAR_H

#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/pattern_generator.h"
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
    bool is_goal_violation;
    int solution_index;
    int variable;

    Flaw(bool is_goal_violation, int solution_index, int variable)
        : is_goal_violation(is_goal_violation)
        , solution_index(solution_index)
        , variable(variable)
    {
    }
};

template <typename PDBType>
class AbstractSolutionData {
    std::unique_ptr<PDBType> pdb;
    std::set<int> blacklist;
    AbstractPolicy policy;
    bool solved;

public:
    AbstractSolutionData(
        const ProbabilisticTaskProxy& task_proxy,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const Pattern& pattern,
        std::set<int> blacklist,
        bool wildcard);

    AbstractSolutionData(
        const ProbabilisticTaskProxy& task_proxy,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const PDBType& previous,
        int add_var,
        std::set<int> blacklist,
        bool wildcard);

    AbstractSolutionData(
        const ProbabilisticTaskProxy& task_proxy,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const PDBType& merge_left,
        const PDBType& merge_right,
        std::set<int> blacklist,
        bool wildcard);

    const Pattern& get_pattern() const;

    void blacklist_variable(int var);

    bool is_blacklisted(int var) const;

    const std::set<int>& get_blacklist() const;

    const PDBType& get_pdb() const;

    std::unique_ptr<PDBType> steal_pdb();

    const AbstractPolicy& get_policy() const;

    value_t get_policy_cost() const;

    bool is_solved() const;

    void mark_as_solved();

    bool solution_exists() const;
};

namespace cegar {
template <typename PDBType>
class FlawFindingStrategyFactory;
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
    std::shared_ptr<cegar::FlawFindingStrategyFactory<PDBType>>
        flaw_strategy_factory;

    // behavior defining parameters
    const bool wildcard;

    const int max_refinements;
    const int max_pdb_size;
    const int max_collection_size;

    // set this to true if you want to
    // generate only one pattern
    const bool ignore_goal_violations;

    const bool treat_goal_violations_differently;
    const bool local_blacklisting;
    const int global_blacklist_size;
    const InitialCollectionType initial;
    const int given_goal;
    const utils::Verbosity verbosity;
    const double max_time;

    std::vector<int> remaining_goals;
    std::unordered_set<int> global_blacklist;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<std::unique_ptr<AbstractSolutionData<PDBType>>> solutions;

    // Takes a variable as key and returns the index of the solutions-entry
    // whose pattern contains said variable. Used for checking if a variable
    // is already included in some pattern as well as for quickly finding
    // the other partner for merging.
    std::unordered_map<int, int> solution_lookup;

    int collection_size;

    // If the algorithm finds a single solution instance that solves
    // the concrete problem, then it will store its index here.
    // This enables simpler plan extraction later on.
    int concrete_solution_index;

public:
    explicit PatternCollectionGeneratorCegar(const options::Options& opts);

    PatternCollectionGeneratorCegar(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        std::shared_ptr<SubCollectionFinderFactory>
            subcollection_finder_factory,
        std::shared_ptr<cegar::FlawFindingStrategyFactory<PDBType>>
            flaw_strategy_factory,
        bool wildcard,
        int arg_max_refinements,
        int arg_max_pdb_size,
        int arg_max_collection_size,
        bool arg_ignore_goal_violations,
        bool treat_goal_violations_differently,
        bool arg_local_blacklisting,
        int arg_global_blacklist_size,
        InitialCollectionType arg_initial,
        int given_goal,
        utils::Verbosity verbosity,
        double arg_max_time);

    virtual ~PatternCollectionGeneratorCegar() = default;

    PatternCollectionInformation<PDBType>
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

private:
    void print_collection() const;
    void generate_trivial_solution_collection(
        const ProbabilisticTaskProxy& task_proxy);
    bool time_limit_reached(const utils::CountdownTimer& timer) const;
    bool termination_conditions_met(
        const utils::CountdownTimer& timer,
        int refinement_counter) const;

    /*
      Try to apply the specified abstract policy
      in concrete space by starting with the specified state.
      Return the last state that could be reached before the
      solution failed (if the solution did not fail, then the
      returned state is a goal state of the concrete task).
      The second element of the returned pair is a list of variables
      that caused the solution to fail.
     */
    void apply_policy(
        cegar::FlawFindingStrategy<PDBType>& flaw_strategy,
        int solution_index,
        std::vector<int>& state,
        std::vector<Flaw>& flaws);
    void get_flaws(
        cegar::FlawFindingStrategy<PDBType>& flaw_strategy,
        const State& initial_state,
        std::vector<Flaw>& flaws);

    // Methods related to refining (and adding patterns to the collection
    // generally).
    void update_goals(int added_var);
    bool
    can_add_singleton_pattern(const VariablesProxy& variables, int var) const;
    void add_pattern_for_var(const ProbabilisticTaskProxy& task_proxy, int var);
    void handle_goal_violation(
        const ProbabilisticTaskProxy& task_proxy,
        const VariablesProxy& variables,
        const Flaw& flaw);
    bool can_merge_patterns(int index1, int index2) const;
    void merge_patterns(
        const ProbabilisticTaskProxy& task_proxy,
        int index1,
        int index2);
    bool can_add_variable_to_pattern(
        const VariablesProxy& variables,
        int index,
        int var) const;
    void add_variable_to_pattern(
        const ProbabilisticTaskProxy& task_proxy,
        int index,
        int var);
    void handle_precondition_violation(
        const ProbabilisticTaskProxy& task_proxy,
        const VariablesProxy& variables,
        const Flaw& flaw);
    void refine(
        const ProbabilisticTaskProxy& task_proxy,
        const VariablesProxy& variables,
        const std::vector<Flaw>& flaws);
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
