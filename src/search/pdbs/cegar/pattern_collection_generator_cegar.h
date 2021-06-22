#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H

#include "../pattern_generator.h"

#include "../types.h"

#include "../../utils/rng.h"
#include "../../utils/logging.h"
#include "../../global_state.h"
#include "../../options/options.h"
#include "../../globals.h"
#include "../../global_operator.h"

#include <unordered_set>
#include <vector>
#include <string>

namespace options {
class OptionParser;
}

namespace utils {
class CountdownTimer;
}

namespace pdbs {
class AbstractSolutionData;

struct Flaw {
    bool is_goal_violation;
    int solution_index;
    int variable;

    Flaw(bool is_goal_violation, int solution_index, int variable)
        : is_goal_violation(is_goal_violation)
        , solution_index(solution_index)
        , variable(variable) { }
};

using FlawList = std::vector<Flaw>;

struct ExplicitGState {
    std::vector<int> values;

    ExplicitGState(std::vector<int> values)
        : values(std::move(values)) {}

    int& operator[](int i) {
        return values[i];
    }

    const int& operator[](int i) const {
        return values[i];
    }

    ExplicitGState get_successor(const GlobalOperator& op) {
        assert(!op.is_axiom());

        ExplicitGState s(*this);

        for (auto& eff : op.get_effects()) {
            assert(eff.conditions.empty());
            s[eff.var] = eff.val;
        }

        return s;
    }

    bool is_goal() const {
        for (auto& [goal_var, goal_val] : g_goal) {
            if (values[goal_var] != goal_val) {
                return false;
            }
        }

        return true;
    }
};

enum class InitialCollectionType {
    GIVEN_GOAL,
    RANDOM_GOAL,
    ALL_GOALS
};

class PatternCollectionGeneratorCegar : public PatternCollectionGenerator {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    // behavior defining parameters
    const int max_refinements;
    const int max_pdb_size;
    const int max_collection_size;
    const bool extended_plans; // this is passed to AbstractSolutionData constructors, to set what kind of plan should be generated
    const bool ignore_goal_violations; // set this to true if you want to generate only one pattern
    const bool treat_goal_violations_differently;
    const bool local_blacklisting;
    const int global_blacklist_size;
    const InitialCollectionType initial;
    const int given_goal;
    const utils::Verbosity verbosity;
    const double max_time;

    const std::string token = "CEGAR_PDBs: ";
    // The value of this constant tells the algorithm that the abstract
    // solution has failed in the concrete state-space because some
    // unknown goal variable was not satisfied.
    // If this value shows up in the list of flaw variables, then it is
    // a signal for the algorithm to randomly (or intelligently) choose
    // a goal variable that is not yet in the pattern (collection) and
    // use it for refinement.
    const int RANDOM_GOAL_VARIABLE = -1;

    std::vector<int> remaining_goals;
    std::unordered_set<int> global_blacklist;

    // the pattern collection in form of their pdbs plus stored plans.
    std::vector<std::unique_ptr<AbstractSolutionData>> solutions;
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

    void print_collection() const;
    void generate_trivial_solution_collection(OperatorCost cost_type);
    bool time_limit_reached(const utils::CountdownTimer &timer) const;
    bool termination_conditions_met(
        const utils::CountdownTimer &timer, int refinement_counter) const;

    /*
      Try to apply the specified abstract solution
      in concrete space by starting with the specified state.
      Return the last state that could be reached before the
      solution failed (if the solution did not fail, then the
      returned state is a goal state of the concrete task).
      The second element of the returned pair is a list of variables
      that caused the solution to fail.
     */
    FlawList apply_extended_plan(
        int solution_index, const ExplicitGState &init);
    FlawList get_flaws();

    // Methods related to refining (and adding patterns to the collection generally).
    void update_goals(int added_var);
    bool can_add_singleton_pattern(int var) const;
    void add_pattern_for_var(OperatorCost cost_type, int var);
    void handle_goal_violation(OperatorCost cost_type, const Flaw &flaw);
    bool can_merge_patterns(int index1, int index2) const;
    void merge_patterns(OperatorCost cost_type, int index1, int index2);
    bool can_add_variable_to_pattern(int index, int var) const;
    void add_variable_to_pattern(OperatorCost cost_type, int index, int var);
    void handle_precondition_violation(
        OperatorCost cost_type, const Flaw &flaw);
    void refine(OperatorCost cost_type, const FlawList& flaws);

public:
    explicit PatternCollectionGeneratorCegar(const options::Options &opts);

    PatternCollectionGeneratorCegar(
        const std::shared_ptr<utils::RandomNumberGenerator> &arg_rng,
        int arg_max_refinements,
        int arg_max_pdb_size,
        int arg_max_collection_size,
        bool arg_extended_plans,
        bool arg_ignore_goal_violations,
        bool treat_goal_violations_differently,
        bool arg_local_blacklisting,
        int arg_global_blacklist_size,
        InitialCollectionType arg_initial,
        int given_goal,
        utils::Verbosity verbosity,
        double arg_max_time);

    virtual ~PatternCollectionGeneratorCegar();

    PatternCollectionInformation generate(OperatorCost cost_type) override;
};

extern void add_pattern_collection_generator_cegar_options_to_parser(
    options::OptionParser &parser);
}


#endif //PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
