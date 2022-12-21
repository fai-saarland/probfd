#ifndef LEGACY_MERGE_AND_SHRINK_ABSTRACTION_H
#define LEGACY_MERGE_AND_SHRINK_ABSTRACTION_H

#include "legacy/merge_and_shrink/label_reducer.h"
#include "legacy/merge_and_shrink/shrink_strategy.h"

//#include "merge_and_shrink_heuristic.h" // needed for MergeStrategy type;
// TODO: move that type somewhere else?

#include "operator_cost.h"

#include <iostream>
#include <list>
#include <set>
#include <vector>


using namespace std;

class GlobalState;
class GlobalOperator;

namespace legacy {
namespace merge_and_shrink {

void find_sccs(
    vector<int>& stack,
    vector<bool>& in_stack,
    vector<std::list<AbstractStateRef>>& final_sccs,
    const vector<vector<int>>& adjacency_matrix,
    vector<int>& indices,
    vector<int>& lowlinks,
    int& index,
    const int state,
    const bool all_goal_vars_in,
    vector<bool>& is_goal,
    vector<int>& state_to_scc,
    const bool find_only_one = false);

struct ArrayList {
    ArrayList* prev;
    ArrayList* succ;
    int op_no;
};

struct AbstractTransition {
    AbstractStateRef src;
    AbstractStateRef target;

    AbstractTransition(AbstractStateRef src_, AbstractStateRef target_)
        : src(src_)
        , target(target_)
    {
    }

    bool operator==(const AbstractTransition& other) const
    {
        return src == other.src && target == other.target;
    }

    bool operator!=(const AbstractTransition& other) const
    {
        return !(*this == other);
    }

    bool operator<(const AbstractTransition& other) const
    {
        return src < other.src || (src == other.src && target < other.target);
    }

    void dump() const
    {
        cout << "Transition from " << src << " to " << target << endl;
    }
};

class Abstraction {
public:
    static const int PRUNED_STATE;
    static const int DISTANCE_UNKNOWN;

private:
    friend class AtomicAbstraction;
    friend class CompositeAbstraction;

    friend class ShrinkStrategy; // for apply() -- TODO: refactor!

    const bool is_unit_cost;
    const OperatorCost cost_type;

    // Allows us to disable one variable. By default is false, unless
    // the abstraction size gets reduced to 1 or we explicity mark the
    // abstraction as irrelevant. NOTE: use is_irrelevant() to check
    // if the abstraction is irrelevant because it also looks whether
    // num_states is only 1.
    bool irrelevant;

    vector<const GlobalOperator*> relevant_operators;
    int num_states;
    vector<vector<AbstractTransition>> transitions_by_op;
    vector<ArrayList*> non_empty_transitions;
    ArrayList* first_non_empty_transition;

    vector<int> init_distances;
    vector<int> goal_distances;
    vector<bool> goal_states;
    AbstractStateRef init_state;

    int max_f;
    int max_g;
    int max_h;

    bool are_labels_reduced;

    mutable int peak_memory;
    mutable int max_num_states;
    mutable int max_num_transitions;
    mutable int overall_peak_memory;
    mutable int overall_max_num_states;
    mutable int overall_max_num_transitions;
    mutable bool all_goal_vars_in;

    bool last_computation_used_uniform_costs;

    // Information got in normalize to be used by
    // count_transitions_var and count_transitions_var_empty. For each operator
    // it has a set of variables involved in its label.
    std::vector<std::set<int>> vars_label_operators;

    std::vector<double> number_transitions_var;
    std::vector<double> number_transitions_var_empty;
    std::vector<double> number_transitions_var_empty_goal;

    void clear_distances();
    void compute_init_distances_unit_cost();
    void compute_goal_distances_unit_cost();
    void compute_init_distances_general_cost();
    void compute_goal_distances_general_cost();

    void
    apply_abstraction(vector<std::list<AbstractStateRef>>& collapsed_groups);
    typedef std::list<AbstractStateRef> EquivalenceClass;
    typedef std::vector<EquivalenceClass> EquivalenceRelation;
    bool apply_label_inheritance(
        const LabelReducer* reducer,
        bool new_approach = false);
    void comp_tc(
        AbstractStateRef v,
        int* current_index,
        vector<int>& index,
        vector<AbstractStateRef>& nstack,
        vector<int>& cstack,
        vector<int>& C,
        vector<int>& root,
        vector<vector<AbstractStateRef>>& successors,
        int* component_count,
        vector<set<int>>& csucc,
        EquivalenceRelation& sccs);

    int total_transitions() const;
    int unique_unlabeled_transitions() const;

protected:
    vector<int> varset;

    virtual void apply_abstraction_to_lookup_table(
        const vector<AbstractStateRef>& abstraction_mapping) = 0;
    virtual int memory_estimate() const;

public:
    Abstraction(bool is_unit_cost, OperatorCost cost_type);
    virtual ~Abstraction();
    virtual AbstractStateRef
    get_abstract_state(const GlobalState& state) const = 0;

    // Two methods to identify the abstraction in output.
    // tag is a convience method that upper-cases the first letter of
    // description and appends ": ";
    virtual std::string description() const = 0;
    std::string tag() const;

    AbstractStateRef get_initial_state() const { return init_state; }

    static void build_atomic_abstractions(
        bool is_unit_cost,
        OperatorCost cost_type,
        std::vector<Abstraction*>& result);

    bool is_irrelevant() const { return irrelevant; }

    void mark_irrelevant()
    {
        irrelevant = true;
        release_memory();
    }

    bool is_solvable() const;

    int get_cost(const GlobalState& state) const;
    int size() const;
    void statistics(bool include_expensive_statistics) const;

    int get_peak_memory_estimate() const;
    // NOTE: This will only return something useful if the memory estimates
    //       have been computed along the way by calls to statistics().
    // TODO: Find a better way of doing this that doesn't require
    //       a mutable attribute?
    int get_max_num_states() const;
    int get_max_num_transitions() const;
    int get_overall_peak_memory_estimate() const;
    int get_overall_max_num_states() const;
    int get_overall_max_num_transitions() const;
    void set_overall_peak_memory_estimate(int peak_memory);
    void set_overall_max_num_states(int max_num_states);
    void set_overall_max_num_transitions(int max_num_transitions);

    void count_transitions();

    const std::vector<double>& get_count_transitions_var()
    {
        if (number_transitions_var.empty()) {
            count_transitions();
        }
        return number_transitions_var;
    }

    const std::vector<double>& get_count_transitions_var_empty()
    {
        if (number_transitions_var_empty.empty()) {
            count_transitions();
        }
        return number_transitions_var_empty;
    }

    const std::vector<double>& get_count_transitions_var_empty_goal()
    {
        if (number_transitions_var_empty_goal.empty()) {
            count_transitions();
        }
        return number_transitions_var_empty_goal;
    }

    bool is_in_varset(int var) const;

    void compute_distances(
        bool prune_unreachable,
        bool prune_irrelevant,
        bool continue_if_unreachable = false,
        bool use_uniform_costs = false,
        bool force_recomputations = false);
    void normalize(
        bool reduce_labels,
        const std::vector<bool>& caught_labels,
        bool use_label_inheritance = false,
        bool new_approach = false);
    void release_memory();

    void dump() const;
    void dump(ostream& ou) const; // For testing
    // Michael: for label subset bisimulation
    void dump_goal_leading_actions(bool backward_pruning = false) const;
    int set_goal_leading_actions(
        vector<bool>& labels,
        bool backward_pruning = false) const;
    int set_shortest_path_relevant_actions(
        vector<bool>& labels,
        bool backward_pruning = false);
    int set_path_relevant_actions(
        vector<bool>& labels,
        bool backward_pruning = false);
    void get_goal_leading_actions(set<int>& ops) const;
    void dump_actions_with_goal_distances() const;

    // The following methods exist for the benefit of shrink strategies.
    int get_max_f() const;
    int get_max_g() const;
    int get_max_h() const;

    OperatorCost get_cost_type() const { return cost_type; }

    bool get_all_goal_vars_in() const { return all_goal_vars_in; }

    bool is_goal_state(int state) const { return goal_states[state]; }

    int get_init_distance(int state) const { return init_distances[state]; }

    int get_goal_distance(int state) const { return goal_distances[state]; }

    int get_num_ops() const { return transitions_by_op.size(); }

    std::vector<std::vector<AbstractTransition>> extract_transitions()
    {
        std::vector<std::vector<AbstractTransition>> result(
            std::move(transitions_by_op));
        transitions_by_op.clear();
        return result;
    }

    const std::vector<AbstractTransition>&
    get_transitions_for_op(int op_no) const
    {
        return transitions_by_op[op_no];
    }

    const std::vector<int>& get_varset() const { return varset; }

    const std::vector<const GlobalOperator*>& get_relevant_operators() const
    {
        return relevant_operators;
    }

    inline bool are_abstraction_labels_reduced() const
    {
        return are_labels_reduced;
    }

    bool is_scc() const;

    void
    write_mip_file(const bool backward_pruning, const bool use_uniform_costs);
    void write_pmaxsat_file(const bool backward_pruning);
    void write_action_files(const bool backward_pruning);

    friend std::ostream&
    operator<<(std::ostream& stream, const Abstraction& abstraction);

    //    void remove_goal_outgoing_transitions();
};

class AtomicAbstraction : public Abstraction {
    int variable;
    vector<AbstractStateRef> lookup_table;

protected:
    virtual std::string description() const;
    virtual void apply_abstraction_to_lookup_table(
        const vector<AbstractStateRef>& abstraction_mapping);
    virtual int memory_estimate() const;

public:
    virtual AbstractStateRef get_abstract_state(const GlobalState& state) const;
    AtomicAbstraction(bool is_unit_cost, OperatorCost cost_type, int variable);
    virtual ~AtomicAbstraction();
};

class CompositeAbstraction : public Abstraction {
    Abstraction* components[2];
    vector<vector<AbstractStateRef>> lookup_table;

protected:
    virtual std::string description() const;
    virtual void apply_abstraction_to_lookup_table(
        const vector<AbstractStateRef>& abstraction_mapping);
    virtual int memory_estimate() const;

public:
    virtual AbstractStateRef get_abstract_state(const GlobalState& state) const;
    CompositeAbstraction(
        bool is_unit_cost,
        OperatorCost cost_type,
        Abstraction* abs1,
        Abstraction* abs2);
    virtual ~CompositeAbstraction();
};

} // namespace merge_and_shrink_legacy
} // namespace legacy

#endif
