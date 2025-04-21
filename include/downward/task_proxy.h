#ifndef DOWNWARD_TASK_PROXY_H
#define DOWNWARD_TASK_PROXY_H

#include "downward/abstract_task.h"
#include "downward/axioms.h"
#include "downward/operator_id.h"
#include "downward/proxy_collection.h"
#include "downward/state_id.h"
#include "downward/task_id.h"

#include "downward/algorithms/int_packer.h"
#include "downward/utils/collections.h"
#include "downward/utils/hash.h"
#include "downward/utils/system.h"

#include "probfd/views/convert.h"
#include "probfd/views/enumerate.h"

#include <cassert>
#include <compare>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

namespace downward {
class AxiomsProxy;
class AxiomEffectProxy;
class AxiomEffectConditionsProxy;
class AxiomEffectsProxy;
class EffectProxy;
class EffectConditionsProxy;
class EffectsProxy;
class FactProxy;
class FactsProxy;
class GoalsProxy;
class OperatorEffectProxy;
class OperatorEffectConditionsProxy;
class OperatorEffectsProxy;
class OperatorProxy;
class OperatorsProxy;
class PartialOperatorsProxy;
class PreconditionsProxy;
class State;
class StateRegistry;
class PlanningTaskProxy;
class TaskProxy;
class VariableProxy;
class VariablesProxy;
} // namespace downward

namespace downward::causal_graph {
class CausalGraph;
}

namespace downward {
using PackedStateBin = int_packer::IntPacker::Bin;

class FactProxy {
    const VariableSpace* task;
    FactPair fact;

public:
    FactProxy(const VariableSpace& task, int var_id, int value);
    FactProxy(const VariableSpace& task, const FactPair& fact);

    ~FactProxy() = default;

    VariableProxy get_variable() const;

    int get_value() const { return fact.value; }

    FactPair get_pair() const { return fact; }

    std::string get_name() const { return task->get_fact_name(fact); }

    friend bool operator==(const FactProxy& left, const FactProxy& right)
    {
        assert(left.task == right.task);
        return left.fact == right.fact;
    }
};

/*
  Proxy class for the collection of all facts of a task.

  We don't implement size() because it would not be constant-time.

  FactsProxy supports iteration, e.g. for range-based for loops. This
  iterates over all facts in order of increasing variable ID, and in
  order of increasing value for each variable.
*/
class FactsProxy {
    class FactsProxyIterator {
        const VariableSpace* task;
        int var_id;
        int value;

    public:
        FactsProxyIterator(const VariableSpace& task, int var_id, int value)
            : task(&task)
            , var_id(var_id)
            , value(value)
        {
        }

        FactProxy operator*() const { return FactProxy(*task, var_id, value); }

        FactsProxyIterator& operator++()
        {
            assert(var_id < task->get_num_variables());
            int num_facts = task->get_variable_domain_size(var_id);
            assert(value < num_facts);
            ++value;
            if (value == num_facts) {
                ++var_id;
                value = 0;
            }
            return *this;
        }

        friend bool operator==(
            const FactsProxyIterator& left,
            const FactsProxyIterator& right)
        {
            assert(left.task == right.task);
            return left.var_id == right.var_id && left.value == right.value;
        }
    };

    const VariableSpace* var_space;

public:
    explicit FactsProxy(const VariableSpace& var_space)
        : var_space(&var_space)
    {
    }

    FactsProxyIterator begin() const
    {
        return FactsProxyIterator(*var_space, 0, 0);
    }

    FactsProxyIterator end() const
    {
        return FactsProxyIterator(
            *var_space,
            var_space->get_num_variables(),
            0);
    }
};

class VariableProxy {
    const VariableSpace* var_space;
    int id;

public:
    VariableProxy(const VariableSpace& var_space, int id)
        : var_space(&var_space)
        , id(id)
    {
    }

    int get_id() const { return id; }

    std::string get_name() const { return var_space->get_variable_name(id); }

    int get_domain_size() const
    {
        return var_space->get_variable_domain_size(id);
    }

    FactProxy get_fact(int index) const
    {
        assert(index < get_domain_size());
        return FactProxy(*var_space, id, index);
    }

    friend bool
    operator==(const VariableProxy& left, const VariableProxy& right)
    {
        assert(left.var_space == right.var_space);
        return left.id == right.id;
    }
};

class VariablesProxy : public ProxyCollection<VariablesProxy> {
    const VariableSpace* var_space;

public:
    explicit VariablesProxy(const PlanningTask& var_space)
        : var_space(&var_space)
    {
    }

    std::size_t size() const { return var_space->get_num_variables(); }

    VariableProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return VariableProxy(*var_space, index);
    }

    FactsProxy get_facts() const { return FactsProxy(*var_space); }
};

class AxiomPreconditionsProxy
    : public ProxyCollection<AxiomPreconditionsProxy> {
    const AxiomSpace* axiom_space;
    int axiom_index;

public:
    AxiomPreconditionsProxy(const AxiomSpace& axiom_space, int axiom_index)
        : axiom_space(&axiom_space)
        , axiom_index(axiom_index)
    {
    }

    std::size_t size() const
    {
        return axiom_space->get_num_axiom_preconditions(axiom_index);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        assert(fact_index < size());
        return axiom_space->get_axiom_precondition(axiom_index, fact_index);
    }
};

class AxiomEffectConditionsProxy
    : public ProxyCollection<AxiomEffectConditionsProxy> {
    const AxiomSpace* axiom_space;
    int axiom_index;
    int eff_index;

public:
    AxiomEffectConditionsProxy(
        const AxiomSpace& axiom_space,
        int axiom_index,
        int eff_index)
        : axiom_space(&axiom_space)
        , axiom_index(axiom_index)
        , eff_index(eff_index)
    {
    }

    std::size_t size() const
    {
        return axiom_space->get_num_axiom_effect_conditions(
            axiom_index,
            eff_index);
    }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return axiom_space->get_axiom_effect_condition(
            axiom_index,
            eff_index,
            index);
    }
};

class AxiomEffectProxy {
    const AxiomSpace* axiom_space;
    int axiom_index;
    int eff_index;

public:
    AxiomEffectProxy(
        const AxiomSpace& axiom_space,
        int axiom_index,
        int eff_index)
        : axiom_space(&axiom_space)
        , axiom_index(axiom_index)
        , eff_index(eff_index)
    {
    }

    AxiomEffectConditionsProxy get_conditions() const
    {
        return AxiomEffectConditionsProxy(*axiom_space, axiom_index, eff_index);
    }

    FactPair get_fact() const
    {
        return axiom_space->get_axiom_effect(axiom_index, eff_index);
    }
};

class AxiomEffectsProxy : public ProxyCollection<AxiomEffectsProxy> {
    const AxiomSpace* axiom_space;
    int op_index;

public:
    AxiomEffectsProxy(const AxiomSpace& axiom_space, int op_index)
        : axiom_space(&axiom_space)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return axiom_space->get_num_axiom_effects(op_index);
    }

    AxiomEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return AxiomEffectProxy(*axiom_space, op_index, eff_index);
    }
};

class AxiomProxy {
    const AxiomSpace* axiom_space;
    int index;

public:
    AxiomProxy(const AxiomSpace& axiom_space, int index)
        : axiom_space(&axiom_space)
        , index(index)
    {
    }

    AxiomPreconditionsProxy get_preconditions() const
    {
        return AxiomPreconditionsProxy(*axiom_space, index);
    }

    AxiomEffectsProxy get_effects() const
    {
        return AxiomEffectsProxy(*axiom_space, index);
    }

    int get_cost() const { return 0; }

    std::string get_name() const { return axiom_space->get_axiom_name(index); }

    int get_id() const { return index; }

    friend bool operator==(const AxiomProxy& left, const AxiomProxy& right)
    {
        assert(left.axiom_space == right.axiom_space);
        return left.index == right.index;
    }
};

class AxiomsProxy : public ProxyCollection<AxiomsProxy> {
    const AxiomSpace* axiom_space;

public:
    explicit AxiomsProxy(const AxiomSpace& axiom_space)
        : axiom_space(&axiom_space)
    {
    }

    bool is_derived(int var) const
    {
        int axiom_layer = axiom_space->get_variable_axiom_layer(var);
        return axiom_layer != -1;
    }

    bool is_derived(VariableProxy var) const
    {
        return is_derived(var.get_id());
    }

    int get_axiom_layer(int var) const
    {
        int axiom_layer = axiom_space->get_variable_axiom_layer(var);
        /*
          This should only be called for derived variables.
          Non-derived variables have axiom_layer == -1.
          Use var.is_derived() to check.
        */
        assert(axiom_layer >= 0);
        return axiom_layer;
    }

    int get_axiom_layer(VariableProxy var) const
    {
        return get_axiom_layer(var.get_id());
    }

    int get_default_axiom_value(int var) const
    {
        assert(is_derived(var));
        return axiom_space->get_variable_default_axiom_value(var);
    }

    int get_default_axiom_value(VariableProxy var) const
    {
        return get_default_axiom_value(var.get_id());
    }

    std::size_t size() const { return axiom_space->get_num_axioms(); }

    AxiomProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return AxiomProxy(*axiom_space, index);
    }
};

class OperatorPreconditionsProxy
    : public ProxyCollection<OperatorPreconditionsProxy> {
    const PlanningTask* task;
    int op_index;

public:
    OperatorPreconditionsProxy(const PlanningTask& task, int op_index)
        : task(&task)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_preconditions(op_index);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        assert(fact_index < size());
        return task->get_operator_precondition(op_index, fact_index);
    }
};

class OperatorEffectConditionsProxy
    : public ProxyCollection<OperatorEffectConditionsProxy> {
    const AbstractTask* task;
    int op_index;
    int eff_index;

public:
    OperatorEffectConditionsProxy(
        const AbstractTask& task,
        int op_index,
        int eff_index)
        : task(&task)
        , op_index(op_index)
        , eff_index(eff_index)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_effect_conditions(op_index, eff_index);
    }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return task->get_operator_effect_condition(op_index, eff_index, index);
    }
};

class OperatorEffectProxy {
    const AbstractTask* task;
    int op_index;
    int eff_index;

public:
    OperatorEffectProxy(const AbstractTask& task, int op_index, int eff_index)
        : task(&task)
        , op_index(op_index)
        , eff_index(eff_index)
    {
    }

    OperatorEffectConditionsProxy get_conditions() const
    {
        return OperatorEffectConditionsProxy(*task, op_index, eff_index);
    }

    FactPair get_fact() const
    {
        return task->get_operator_effect(op_index, eff_index);
    }
};

class OperatorEffectsProxy : public ProxyCollection<OperatorEffectsProxy> {
    const AbstractTask* task;
    int op_index;

public:
    OperatorEffectsProxy(const AbstractTask& task, int op_index)
        : task(&task)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_effects(op_index);
    }

    OperatorEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return OperatorEffectProxy(*task, op_index, eff_index);
    }
};

class PartialOperatorProxy {
protected:
    const PlanningTask* task;
    int index;

public:
    PartialOperatorProxy(const PlanningTask& task, int index)
        : task(&task)
        , index(index)
    {
    }

    OperatorPreconditionsProxy get_preconditions() const
    {
        return OperatorPreconditionsProxy(*task, index);
    }

    std::string get_name() const { return task->get_operator_name(index); }

    int get_id() const { return index; }

    friend bool operator==(
        const PartialOperatorProxy& left,
        const PartialOperatorProxy& right)
    {
        assert(left.task == right.task);
        return left.index == right.index;
    }
};

class OperatorProxy : public PartialOperatorProxy {
public:
    OperatorProxy(const AbstractTask& task, int index)
        : PartialOperatorProxy(task, index)
    {
    }

    OperatorEffectsProxy get_effects() const
    {
        return OperatorEffectsProxy(
            *static_cast<const AbstractTask*>(task),
            index);
    }

    int get_cost() const
    {
        return static_cast<const AbstractTask*>(task)->get_operator_cost(index);
    }

    State get_unregistered_successor(
        const State& state,
        AxiomEvaluator& axiom_evaluator) const;
};

class OperatorsProxy : public ProxyCollection<OperatorsProxy> {
    const AbstractTask* task;

public:
    explicit OperatorsProxy(const AbstractTask& task)
        : task(&task)
    {
    }

    std::size_t size() const { return task->get_num_operators(); }

    OperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return OperatorProxy(*task, index);
    }

    OperatorProxy operator[](OperatorID id) const
    {
        return (*this)[id.get_index()];
    }
};

class PartialOperatorsProxy : public ProxyCollection<PartialOperatorsProxy> {
    const PlanningTask* task;

public:
    explicit PartialOperatorsProxy(const PlanningTask& task)
        : task(&task)
    {
    }

    std::size_t size() const { return task->get_num_operators(); }

    PartialOperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return PartialOperatorProxy(*task, index);
    }

    PartialOperatorProxy operator[](OperatorID id) const
    {
        return (*this)[id.get_index()];
    }
};

class PreconditionsProxy : public ProxyCollection<PreconditionsProxy> {
    std::variant<AxiomPreconditionsProxy, OperatorPreconditionsProxy> proxy;

public:
    PreconditionsProxy(AxiomPreconditionsProxy proxy)
        : proxy(proxy)
    {
    }

    PreconditionsProxy(OperatorPreconditionsProxy proxy)
        : proxy(proxy)
    {
    }

    std::size_t size() const
    {
        return std::visit([](const auto& arg) { return arg.size(); }, proxy);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        return std::visit(
            [fact_index](const auto& arg) {
                return arg.operator[](fact_index);
            },
            proxy);
    }
};

class EffectConditionsProxy : public ProxyCollection<EffectConditionsProxy> {
    std::variant<AxiomEffectConditionsProxy, OperatorEffectConditionsProxy>
        proxy;

public:
    EffectConditionsProxy(AxiomEffectConditionsProxy proxy)
        : proxy(proxy)
    {
    }

    EffectConditionsProxy(OperatorEffectConditionsProxy proxy)
        : proxy(proxy)
    {
    }

    std::size_t size() const
    {
        return std::visit([](const auto& arg) { return arg.size(); }, proxy);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        return std::visit(
            [fact_index](const auto& arg) {
                return arg.operator[](fact_index);
            },
            proxy);
    }
};

class EffectProxy {
    std::variant<AxiomEffectProxy, OperatorEffectProxy> proxy;

public:
    EffectProxy(AxiomEffectProxy proxy)
        : proxy(proxy)
    {
    }

    EffectProxy(OperatorEffectProxy proxy)
        : proxy(proxy)
    {
    }

    EffectConditionsProxy get_conditions() const
    {
        return std::visit(
            [](const auto& arg) {
                return EffectConditionsProxy(arg.get_conditions());
            },
            proxy);
    }

    FactPair get_fact() const
    {
        return std::visit(
            [](const auto& arg) { return arg.get_fact(); },
            proxy);
    }
};

class EffectsProxy : public ProxyCollection<EffectsProxy> {
    std::variant<AxiomEffectsProxy, OperatorEffectsProxy> proxy;

public:
    EffectsProxy(AxiomEffectsProxy proxy)
        : proxy(proxy)
    {
    }

    EffectsProxy(OperatorEffectsProxy proxy)
        : proxy(proxy)
    {
    }

    std::size_t size() const
    {
        return std::visit([](const auto& arg) { return arg.size(); }, proxy);
    }

    EffectProxy operator[](std::size_t eff_index) const
    {
        return std::visit(
            [eff_index](const auto& arg) {
                return EffectProxy(arg.operator[](eff_index));
            },
            proxy);
    }
};

class AxiomOrOperatorProxy {
    std::variant<AxiomProxy, OperatorProxy> proxy;

public:
    AxiomOrOperatorProxy(AxiomProxy proxy)
        : proxy(proxy)
    {
    }

    AxiomOrOperatorProxy(OperatorProxy proxy)
        : proxy(proxy)
    {
    }

    operator AxiomProxy() const
    {
        assert(is_axiom());
        return std::get<AxiomProxy>(proxy);
    }

    operator OperatorProxy() const
    {
        assert(!is_axiom());
        return std::get<OperatorProxy>(proxy);
    }

    bool is_axiom() const { return proxy.index() == 0; }

    PreconditionsProxy get_preconditions() const
    {
        return std::visit(
            [](const auto& arg) {
                return PreconditionsProxy(arg.get_preconditions());
            },
            proxy);
    }

    EffectsProxy get_effects() const
    {
        return std::visit(
            [](const auto& arg) { return EffectsProxy(arg.get_effects()); },
            proxy);
    }

    int get_cost() const
    {
        return std::visit(
            [](const auto& arg) { return arg.get_cost(); },
            proxy);
    }

    std::string get_name() const
    {
        return std::visit(
            [](const auto& arg) { return arg.get_name(); },
            proxy);
    }

    int get_id() const
    {
        return std::visit([](const auto& arg) { return arg.get_id(); }, proxy);
    }

    friend bool operator==(
        const AxiomOrOperatorProxy& left,
        const AxiomOrOperatorProxy& right)
    {
        return left.proxy == right.proxy;
    }
};

class GoalsProxy : public ProxyCollection<GoalsProxy> {
    const GoalFactList* goal_fact_list;

public:
    explicit GoalsProxy(const GoalFactList& goal_fact_list)
        : goal_fact_list(&goal_fact_list)
    {
    }

    std::size_t size() const { return goal_fact_list->get_num_goals(); }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return goal_fact_list->get_goal_fact(index);
    }
};

extern void
apply_axioms(const PlanningTaskProxy& task_proxy, std::vector<int>& values);

template <typename T>
concept OperatorLike =
    requires(const T& op, const State& state, AxiomEvaluator& axiom_evaluator) {
        {
            op.get_unregistered_successor(state, axiom_evaluator)
        } -> std::same_as<State>;
    };

template <std::ranges::input_range FactRange, typename StateLike>
bool all_facts_true(const FactRange& facts, const StateLike& state)
    requires std::
        convertible_to<std::ranges::range_reference_t<FactRange>, FactPair>
{
    for (const auto [var, value] : facts) {
        if (state[var] != value) return false;
    }
    return true;
}

template <typename ConditionalEffects, typename StateLike>
void apply_conditional_effects(
    const ConditionalEffects& effects,
    const StateLike& condition_state,
    std::vector<int>& values)
{
    for (const auto effect : effects) {
        if (all_facts_true(effect.get_conditions(), condition_state)) {
            const auto [var, value] = effect.get_fact();
            values[var] = value;
        }
    }
}

class State : public ProxyCollection<State> {
    /*
      TODO: We want to try out two things:
        1. having StateID and num_variables next to each other, so that they
           can fit into one 8-byte-aligned block.
        2. removing num_variables altogether, getting it on demand.
           It is not used in (very) performance-critical code.
      Perhaps 2) has no positive influence after 1) because we would just have
      a 4-byte gap at the end of the object. But it would probably be nice to
      have fewer attributes regardless.
    */
    const StateRegistry* registry;
    StateID id;
    const PackedStateBin* buffer;
    /*
      values is mutable because we think of it as a redundant representation
      of the state's contents, a kind of cache. One could argue for doing this
      differently (for example because some methods require unpacked data, so
      in some sense its presence changes the "logical state" from the program
      perspective. It is a bit weird to have a const function like unpack that
      is only called for its side effect on the object. But we decided to use
      const here to mean "const from the perspective of the state space
      semantics of the state".
    */
    mutable std::shared_ptr<std::vector<int>> values;
    const int_packer::IntPacker* state_packer;
    int num_variables;

public:
    // Construct a registered state with only packed data.
    State(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer);

    // Construct a registered state with packed and unpacked data.
    State(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer,
        std::vector<int>&& values);

    // Construct a state with only unpacked data.
    explicit State(std::vector<int>&& values);

    /* Generate unpacked data if it is not available yet. Calling the function
       on a state that already has unpacked data has no effect. */
    void unpack() const;

    std::size_t size() const;

    int operator[](std::size_t var_id) const;
    int operator[](VariableProxy var) const;

    /* Return a pointer to the registry in which this state is registered.
       If the state is not registered, return nullptr. */
    const StateRegistry* get_registry() const;

    /* Return the ID of the state within its registry. If the state is not
       registered, return StateID::no_state. */
    StateID get_id() const;

    /* Access the unpacked values. Accessing the unpacked values in a state
       that doesn't have them is an error. Use unpack() to ensure the data
       exists. */
    const std::vector<int>& get_unpacked_values() const;

    /* Access the packed values. Accessing packed values on states that do
       not have them (unregistered states) is an error. */
    const PackedStateBin* get_buffer() const;

    template <OperatorLike O>
    State
    get_unregistered_successor(AxiomEvaluator& axiom_evaluator, const O& op)
        const
    {
        return op.get_unregistered_successor(*this, axiom_evaluator);
    }

    template <typename ConditionalEffects>
    State get_unregistered_successor(
        AxiomEvaluator& axiom_evaluator,
        const ConditionalEffects& effects) const
    {
        assert(values);
        std::vector<int> new_values = get_unpacked_values();
        apply_conditional_effects(effects, *this, new_values);
        axiom_evaluator.evaluate(new_values);
        return State(std::move(new_values));
    }

    friend bool operator==(const State& left, const State& right);
};

class StateEnumerateFn
    : public std::ranges::range_adaptor_closure<StateEnumerateFn> {
public:
    [[nodiscard]]
    auto operator()(const State& state) const
        noexcept(noexcept(probfd::views::enumerate_view{state}))
    {
        return probfd::views::enumerate_view{state};
    }

    auto operator()(State&& state) const
        noexcept(noexcept(probfd::views::enumerate_view{std::move(state)}))
    {
        return probfd::views::enumerate_view{std::move(state)};
    }
};

inline constexpr auto as_fact_pair_set =
    StateEnumerateFn{} |
    // std::views::transform([](const auto& p) { return FactPair(p);});
    probfd::views::convert<FactPair>;

namespace utils {
inline void feed(HashState& hash_state, const State& state)
{
    /*
      Hashing a state without unpacked data will result in an error.
      We don't want to unpack states implicitly, so this rules out the option
      of unpacking the states here on demand. Mixing hashes from packed and
      unpacked states would lead to logically equal states with different
      hashes. Hashing packed (and therefore registered) states also seems like
      a performance error because it's much cheaper to hash the state IDs
      instead.
    */
    feed(hash_state, state.get_unpacked_values());
}
} // namespace utils

class PlanningTaskProxy {
protected:
    const PlanningTask* task;

public:
    explicit PlanningTaskProxy(const PlanningTask& task)
        : task(&task)
    {
    }

    void subscribe_to_task_destruction(
        subscriber::Subscriber<PlanningTask>* subscriber) const
    {
        task->subscribe(subscriber);
    }

    TaskID get_id() const { return TaskID(task); }

    FactProxy get_fact_proxy(FactPair fact) const
    {
        return FactProxy(*task, fact);
    }

    VariablesProxy get_variables() const { return VariablesProxy(*task); }

    PartialOperatorsProxy get_partial_operators() const
    {
        return PartialOperatorsProxy(*task);
    }

    AxiomsProxy get_axioms() const { return AxiomsProxy(*task); }

    GoalsProxy get_goals() const { return GoalsProxy(*task); }

    State get_initial_state() const
    {
        return State(task->get_initial_state_values());
    }

    explicit operator TaskProxy() const;
};

class TaskProxy : public PlanningTaskProxy {
public:
    explicit TaskProxy(const AbstractTask& task)
        : PlanningTaskProxy(task)
    {
    }

    OperatorsProxy get_operators() const
    {
        return OperatorsProxy(*static_cast<const AbstractTask*>(task));
    }

    const causal_graph::CausalGraph& get_causal_graph() const;
};

inline PlanningTaskProxy::operator TaskProxy() const
{
    return TaskProxy(*static_cast<const AbstractTask*>(task));
}

inline FactProxy::FactProxy(const VariableSpace& task, const FactPair& fact)
    : task(&task)
    , fact(fact)
{
    assert(fact.var >= 0 && fact.var < task.get_num_variables());
    assert(fact.value >= 0 && fact.value < get_variable().get_domain_size());
}

inline FactProxy::FactProxy(const VariableSpace& task, int var_id, int value)
    : FactProxy(task, FactPair(var_id, value))
{
}

inline VariableProxy FactProxy::get_variable() const
{
    return VariableProxy(*task, fact.var);
}

inline bool operator==(const State& left, const State& right)
{
    if (left.registry != right.registry) {
        std::cerr << "Comparing registered states with unregistered states "
                  << "or registered states from different registries is "
                  << "treated as an error because it is likely not "
                  << "intentional." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    if (left.registry) {
        // Both states are registered and from the same registry.
        return left.id == right.id;
    } else {
        // Both states are unregistered.
        assert(left.values);
        assert(right.values);
        return *left.values == *right.values;
    }
}

inline void State::unpack() const
{
    if (!values) {
        int num_variables = size();
        /*
          A micro-benchmark in issue348 showed that constructing the vector
          in the required size and then assigning values was faster than the
          more obvious reserve/push_back. Although, the benchmark did not
          profile this specific code.

          We might consider a bulk-unpack method in state_packer that could be
          more efficient. (One can imagine state packer to have extra data
          structures that exploit sequentially unpacking each entry, by doing
          things bin by bin.)
        */
        values = std::make_shared<std::vector<int>>(num_variables);
        for (int var = 0; var < num_variables; ++var) {
            (*values)[var] = state_packer->get(buffer, var);
        }
    }
}

inline std::size_t State::size() const
{
    return num_variables;
}

inline int State::operator[](std::size_t var_id) const
{
    assert(var_id < size());
    if (values) {
        return (*values)[var_id];
    } else {
        assert(buffer);
        assert(state_packer);
        return state_packer->get(buffer, var_id);
    }
}

inline int State::operator[](VariableProxy var) const
{
    return (*this)[var.get_id()];
}

inline const StateRegistry* State::get_registry() const
{
    return registry;
}

inline StateID State::get_id() const
{
    return id;
}

inline const PackedStateBin* State::get_buffer() const
{
    /*
      TODO: we should profile what happens if we #ifndef NDEBUG this test here
      and in other places (e.g. the next method). The 'if' itself is probably
      not costly, but the 'cerr <<' stuff might prevent inlining.
    */
    if (!buffer) {
        std::cerr << "Accessing the packed values of an unregistered state is "
                  << "treated as an error." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    return buffer;
}

inline const std::vector<int>& State::get_unpacked_values() const
{
    if (!values) {
        std::cerr << "Accessing the unpacked values of a state without "
                  << "unpacking them first is treated as an error. Please "
                  << "use State::unpack first." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    return *values;
}

static_assert(downward::OperatorLike<OperatorProxy>);

} // namespace downward

#endif // DOWNWARD_TASK_PROXY_H
