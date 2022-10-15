#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PATTERN_COLLECTION_GENERATOR_CEGAR_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PATTERN_COLLECTION_GENERATOR_CEGAR_H

#include "probfd/heuristics/pdbs/pattern_selection/pattern_generator.h"

#include "probfd/heuristics/pdbs/pattern_selection/cegar/types.h"

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/globals.h"
#include "probfd/probabilistic_operator.h"

#include "options/options.h"

#include "utils/logging.h"
#include "utils/rng.h"

#include "global_state.h"

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
namespace pattern_selection {

template <typename PDBType>
class AbstractSolutionData;

template <typename PDBType>
class FlawFindingStrategy;

enum class InitialCollectionType { GIVEN_GOAL, RANDOM_GOAL, ALL_GOALS };

template <typename PDBType>
class PatternCollectionGeneratorCegar
    : public PatternCollectionGenerator<PDBType> {
    template <typename>
    friend class BFSFlawFinder;
    template <typename>
    friend class PUCSFlawFinder;
    template <typename>
    friend class SamplingFlawFinder;

    // Random number generator
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Subcollection finder
    std::shared_ptr<SubCollectionFinder> subcollection_finder;

    // Flaw finding strategy
    std::shared_ptr<FlawFindingStrategy<PDBType>> flaw_strategy;

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
        std::shared_ptr<SubCollectionFinder> subcollection_finder,
        std::shared_ptr<FlawFindingStrategy<PDBType>> flaw_finder,
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
    generate(OperatorCost cost_type) override;

private:
    void print_collection() const;
    void generate_trivial_solution_collection();
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
    FlawList apply_policy(int solution_index, const ExplicitGState& init);
    FlawList get_flaws();

    // Methods related to refining (and adding patterns to the collection
    // generally).
    void update_goals(int added_var);
    bool can_add_singleton_pattern(int var) const;
    void add_pattern_for_var(int var);
    void handle_goal_violation(const Flaw& flaw);
    bool can_merge_patterns(int index1, int index2) const;
    void merge_patterns(int index1, int index2);
    bool can_add_variable_to_pattern(int index, int var) const;
    void add_variable_to_pattern(int index, int var);
    void handle_precondition_violation(const Flaw& flaw);
    void refine(const FlawList& flaws);
};

template <typename PDBType>
void add_pattern_collection_generator_cegar_options_to_parser(
    options::OptionParser& parser);

using ExpCostPatternCollectionGeneratorCegar =
    PatternCollectionGeneratorCegar<ExpCostProjection>;
using MaxProbPatternCollectionGeneratorCegar =
    PatternCollectionGeneratorCegar<MaxProbProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
