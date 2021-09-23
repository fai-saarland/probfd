#ifndef PDBS_PATTERN_DATABASE_H
#define PDBS_PATTERN_DATABASE_H

#include "types.h"
#include "../operator_cost.h"

#include <utility>
#include <vector>

class GlobalState;
class GlobalOperator;

namespace utils {
class RandomNumberGenerator;
}

namespace pdbs {
class AbstractOperator {
    /*
      This class represents an abstract operator how it is needed for
      the regression search performed during the PDB-construction. As
      all abstract states are represented as a number, abstract
      operators don't have "usual" effects but "hash effects", i.e. the
      change (as number) the abstract operator implies on a given
      abstract state.
    */
    int concrete_op_id;

    int cost;

    /*
      Preconditions for the regression search, corresponds to normal
      effects and prevail of concrete operators.
    */
    std::vector<FactPair> regression_preconditions;

    /*
      Effect of the operator during regression search on a given
      abstract state number.
    */
    std::size_t hash_effect;
public:
    /*
      Abstract operators are built from concrete operators. The
      parameters follow the usual name convention of SAS+ operators,
      meaning prevail, preconditions and effects are all related to
      progression search.
    */
    AbstractOperator(
        const std::vector<FactPair>& prevail,
        const std::vector<FactPair>& preconditions,
        const std::vector<FactPair>& effects,
        int cost,
        const std::vector<std::size_t>& hash_multipliers,
        int concrete_op_id);
    ~AbstractOperator();

    /*
      Returns variable value pairs which represent the preconditions of
      the abstract operator in a regression search
    */
    const std::vector<FactPair> &get_regression_preconditions() const {
        return regression_preconditions;
    }

    /*
      Returns the effect of the abstract operator in form of a value
      change (+ or -) to an abstract state index
    */
    std::size_t get_hash_effect() const {return hash_effect;}

    int get_concrete_op_id() const { return concrete_op_id; }

    /*
      Returns the cost of the abstract operator (same as the cost of
      the original concrete operator)
    */
    int get_cost() const {return cost;}
    void dump(const Pattern &pattern) const;
};

// Implements a single pattern database
class PatternDatabase {
    Pattern pattern;

    // size of the PDB
    std::size_t num_states;

    /*
      final h-values for abstract-states.
      dead-ends are represented by numeric_limits<int>::max()
    */
    std::vector<int> distances;

    std::vector<std::vector<int>> wildcard_plan;

    // multipliers for each variable for perfect hash function
    std::vector<std::size_t> hash_multipliers;

    /*
      Recursive method; called by build_abstract_operators. In the case
      of a precondition with value = -1 in the concrete operator, all
      multiplied out abstract operators are computed, i.e. for all
      possible values of the variable (with precondition = -1), one
      abstract operator with a concrete value (!= -1) is computed.
    */
    void multiply_out(
        const std::vector<int>&,
        int pos,
        int cost,
        std::vector<FactPair>& prev_pairs,
        std::vector<FactPair>& pre_pairs,
        std::vector<FactPair>& eff_pairs,
        const std::vector<FactPair>& effects_without_pre,
        int concrete_op_id,
        std::vector<AbstractOperator>& operators);

    /*
      Computes all abstract operators for a given concrete operator (by
      its global operator number). Initializes data structures for initial
      call to recursive method multiply_out. variable_to_index maps
      variables in the task to their index in the pattern or -1.
    */
    void build_abstract_operators(
        const GlobalOperator &op, int cost,
        const std::vector<int> &variable_to_index,
        std::vector<AbstractOperator> &operators);

    /*
      Computes all abstract operators, builds the match tree (successor
      generator) and then does a Dijkstra regression search to compute
      all final h-values (stored in distances). operator_costs can
      specify individual operator costs for each operator for action
      cost partitioning. If left empty, default operator costs are used.
    */
    void create_pdb(
        const std::vector<int>& operator_costs,
        bool compute_plan,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool compute_extended_plan);

    /*
      For a given abstract state (given as index), the according values
      for each variable in the state are computed and compared with the
      given pairs of goal variables and values. Returns true iff the
      state is a goal state.
    */
    bool is_goal_state(
        std::size_t state_index,
        const std::vector<FactPair> &abstract_goals) const;

    /*
      The given concrete state is used to calculate the index of the
      according abstract state. This is only used for table lookup
      (distances) during search.
    */
    std::size_t hash_index(const GlobalState &state) const;
    /*
      The given state from the abstract state-space induced by the pattern
      is used to calculate the state's index in the distances lookup-table.
     */
    std::size_t hash_index_abstracted(const std::vector<int> &abs_state) const;
public:
    /*
      Important: It is assumed that the pattern (passed via Options) is
      sorted, contains no duplicates and is small enough so that the
      number of abstract states is below numeric_limits<int>::max()
      Parameters:
       dump:           If set to true, prints the construction time.
       operator_costs: Can specify individual operator costs for each
       operator. This is useful for action cost partitioning. If left
       empty, default operator costs are used.
    */
    PatternDatabase(
        const Pattern& pattern,
        OperatorCost operator_cost,
        bool dump = false,
        bool compute_plan = false,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng = nullptr,
        bool compute_extended_plan = false);
    PatternDatabase(
        const Pattern& pattern,
        bool dump = false,
        const std::vector<int>& operator_costs = std::vector<int>(),
        bool compute_plan = false,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng = nullptr,
        bool compute_extended_plan = false);
    ~PatternDatabase() = default;

    int get_value(const GlobalState &state) const;

    int get_value_abstracted(const std::vector<int> &abstracted_state) const;

    int get_value_for_index(std::size_t index) const;

    bool maps_to(GlobalState &concrete, std::vector<int> &abstracted) {
        return hash_index(concrete) == hash_index_abstracted(abstracted);
    }

    // used for astar search in cegar pdbs
    size_t get_abstract_state_index(const std::vector<int>& abstracted_state) {
        return hash_index_abstracted(abstracted_state);
    }

    // Returns the pattern (i.e. all variables used) of the PDB
    const Pattern &get_pattern() const {
        return pattern;
    }

    // Returns the size (number of abstract states) of the PDB
    int get_size() const {
        return num_states;
    }

    std::vector<std::vector<int>>&& extract_wildcard_plan()
    {
        return std::move(wildcard_plan);
    };

    /*
      Returns the average h-value over all states, where dead-ends are
      ignored (they neither increase the sum of all h-values nor the
      number of entries for the mean value calculation). If all states
      are dead-ends, infinity is returned.
      Note: This is only calculated when called; avoid repeated calls to
      this method!
    */
    double compute_mean_finite_h() const;

    // Returns true iff op has an effect on a variable in the pattern.
    bool is_operator_relevant(const GlobalOperator &op) const;
};
}

#endif
