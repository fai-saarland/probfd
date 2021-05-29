#ifndef TASKS_PDB_ABSTRACTED_TASK_H
#define TASKS_PDB_ABSTRACTED_TASK_H

#include "../../types.h"
#include "../../../global_state.h"
#include "../../../successor_generator.h"

#include <unordered_map>
#include <memory>

class OperatorID {
    int index;

public:
    explicit OperatorID(int index)
        : index(index) {
    }

    static const OperatorID no_operator;

    int get_index() const {
        return index;
    }

    bool operator==(const OperatorID &other) const {
        return index == other.index;
    }

    bool operator!=(const OperatorID &other) const {
        return !(*this == other);
    }

    int hash() const {
        return index;
    }
};

std::ostream &operator<<(std::ostream &os, OperatorID id);

namespace tasks {

using FactPair = std::pair<int, int>;

/*
struct FactPair {
    int var;
    int value;

    FactPair(int var, int value)
        : var(var), value(value) {
    }

    bool operator<(const FactPair &other) const {
        return var < other.var || (var == other.var && value < other.value);
    }

    bool operator==(const FactPair &other) const {
        return var == other.var && value == other.value;
    }

    bool operator!=(const FactPair &other) const {
        return var != other.var || value != other.value;
    }

    /--*
      This special object represents "no such fact". E.g., functions
      that search a fact can return "no_fact" when no matching fact is
      found.
    *--/
    static const FactPair no_fact;
};


std::ostream &operator<<(std::ostream &os, const FactPair &fact_pair);

namespace utils {
inline void feed(HashState &hash_state, const FactPair &fact) {
    feed(hash_state, fact.var);
    feed(hash_state, fact.value);
}
}
*/

class PDBAbstractedTask {
    const pdbs::Pattern& pattern;

    // where to find variable in pattern
    std::unordered_map<int, int> var_to_index;
    
    std::vector<int> operator_indices;
    std::vector<std::vector<FactPair>> operator_preconditions;
    std::vector<std::vector<FactPair>> operator_effects;
    std::vector<FactPair> goals;

    /*
      Convert variable index of the abstracted task to
      the index of the same variable in the original task.
     */
    int get_original_variable_index(int index_in_pattern) const;
    /*
      Convert index with which the variable is accessed in
      the original task to an index under which the variable
      resides in the pattern.
     */
    int get_pattern_variable_index(int index_in_original) const;
    //int get_abstracted_operator_index(int index_in_original) const;
    /*
      Convenience functions for changing the context of a
      given fact between the original and abstracted tasks.
     */
    FactPair convert_from_pattern_fact(const FactPair &fact) const;
    FactPair convert_from_original_fact(const FactPair &fact) const;

public:
    explicit PDBAbstractedTask(const pdbs::Pattern& pattern);

    int get_num_variables() const;
    std::string get_variable_name(int var) const;
    int get_variable_domain_size(int var) const;
    int get_variable_axiom_layer(int var) const;
    int get_variable_default_axiom_value(int var) const;
    std::string get_fact_name(const FactPair &fact) const;

    int get_operator_cost(int index) const;
    std::string get_operator_name(int index) const;
    int get_num_operators() const;
    int get_num_operator_preconditions(int index) const;
    FactPair get_operator_precondition(int op_index, int fact_index) const;
    int get_num_operator_effects(int op_index) const;
    FactPair get_operator_effect(int op_index, int eff_index) const;
    int convert_operator_index_to_parent(int index) const;

    int get_num_goals() const;
    FactPair get_goal_fact(int index) const;

    std::vector<int> get_initial_state_values() const;

    std::vector<int>
    convert_parent_state_values(const GlobalState &state) const;

    void convert_parent_state_values(std::vector<int> &values) const;

    bool is_goal_state(const std::vector<int>& state) const;

    std::vector<int> get_domain_values() const;

    std::unique_ptr<successor_generator::SuccessorGenerator<int>>
    get_successor_generator();
};

}

#endif // TASKS_PDB_ABSTRACTED_TASK_H
