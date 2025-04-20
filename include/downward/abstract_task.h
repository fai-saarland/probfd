#ifndef ABSTRACT_TASK_H
#define ABSTRACT_TASK_H

#include "downward/operator_id.h"

#include "downward/algorithms/subscriber.h"
#include "downward/utils/hash.h"

#include "probfd/concepts.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace downward {
struct FactPair {
    int var;
    int value;

    FactPair(int var, int value)
        : var(var)
        , value(value)
    {
    }

    template <probfd::PairLike P>
        requires(std::convertible_to<std::tuple_element_t<0, P>, int> &&
                 std::convertible_to<std::tuple_element_t<1, P>, int>)
    explicit(false) FactPair(P pair)
        : var(std::get<0>(pair))
        , value(std::get<1>(pair))
    {
    }

    friend auto operator<=>(const FactPair&, const FactPair&) = default;

    /*
      This special object represents "no such fact". E.g., functions
      that search a fact can return "no_fact" when no matching fact is
      found.
    */
    static const FactPair no_fact;
};

std::ostream& operator<<(std::ostream& os, const FactPair& fact_pair);
} // namespace downward

namespace downward::utils {
inline void feed(HashState& hash_state, const FactPair& fact)
{
    feed(hash_state, fact.var);
    feed(hash_state, fact.value);
}
} // namespace downward::utils

namespace downward {

class VariableSpace {
public:
    virtual ~VariableSpace() = default;

    virtual int get_num_variables() const = 0;
    virtual int get_variable_domain_size(int var) const = 0;

    virtual std::string get_variable_name(int var) const = 0;
    virtual std::string get_fact_name(const FactPair& fact) const = 0;
};

class AxiomSpace {
public:
    virtual ~AxiomSpace() = default;

    virtual int get_variable_axiom_layer(int var) const = 0;
    virtual int get_variable_default_axiom_value(int var) const = 0;
    virtual int get_num_axioms() const = 0;
    virtual std::string get_axiom_name(int index) const = 0;
    virtual int get_num_axiom_preconditions(int index) const = 0;
    virtual FactPair
    get_axiom_precondition(int op_index, int fact_index) const = 0;
    virtual int get_num_axiom_effects(int op_index) const = 0;
    virtual int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const = 0;
    virtual FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const = 0;
    virtual FactPair get_axiom_effect(int op_index, int eff_index) const = 0;
};

class GoalFactList {
public:
    virtual ~GoalFactList() = default;

    virtual int get_num_goals() const = 0;
    virtual FactPair get_goal_fact(int index) const = 0;
};

class PlanningTask
    : public subscriber::SubscriberService<PlanningTask>
    , public VariableSpace
    , public AxiomSpace
    , public GoalFactList {
public:
    virtual std::string get_operator_name(int index) const = 0;
    virtual int get_num_operators() const = 0;
    virtual int get_num_operator_preconditions(int index) const = 0;
    virtual FactPair
    get_operator_precondition(int op_index, int fact_index) const = 0;

    virtual std::vector<int> get_initial_state_values() const = 0;
};

class AbstractTask : public PlanningTask {
public:
    virtual int get_operator_cost(int index) const = 0;

    virtual int get_num_operator_effects(int op_index) const = 0;
    virtual int
    get_num_operator_effect_conditions(int op_index, int eff_index) const = 0;
    virtual FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const = 0;
    virtual FactPair get_operator_effect(int op_index, int eff_index) const = 0;
};

} // namespace downward

#endif
