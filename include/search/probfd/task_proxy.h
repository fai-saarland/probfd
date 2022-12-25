#ifndef PROBFD_TASK_PROXY_H
#define PROBFD_TASK_PROXY_H

#include "probfd/probabilistic_task.h"

#include "operator_id.h"
#include "state_id.h"
#include "task_id.h"

#include "../task_proxy.h"

#include "algorithms/int_packer.h"
#include "utils/collections.h"
#include "utils/hash.h"
#include "utils/system.h"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

namespace causal_graph {
class CausalGraph;
}

using PackedStateBin = int_packer::IntPacker::Bin;

namespace probfd {

class AxiomsProxy;
class ProbabilisticEffectProxy;
class ProbabilisticEffectConditionsProxy;
class ProbabilisticEffectsProxy;
class ProbabilisticOperatorProxy;
class ProbabilisticOperatorsProxy;
class ProbabilisticOutcomeProxy;
class ProbabilisticOutcomesProxy;
class PreconditionsProxy;
class State;
class StateRegistry;
class TaskProxy;

template <typename Derived>
class ConditionsProxy {
protected:
    const ProbabilisticTask* task;

public:
    using ItemType = FactProxy;
    explicit ConditionsProxy(const ProbabilisticTask& task)
        : task(&task)
    {
    }

    std::size_t size() const
    {
        return static_cast<const Derived*>(this)->size();
    }

    FactProxy operator[](std::size_t index) const
    {
        return static_cast<const Derived*>(this)->operator[](index);
    }

    bool empty() const { return size() == 0; }
};

class PreconditionsProxy : public ConditionsProxy<PreconditionsProxy> {
    int op_index;
    bool is_axiom;

public:
    PreconditionsProxy(
        const ProbabilisticTask& task,
        int op_index,
        bool is_axiom)
        : ConditionsProxy<PreconditionsProxy>(task)
        , op_index(op_index)
        , is_axiom(is_axiom)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_preconditions(op_index, is_axiom);
    }

    FactProxy operator[](std::size_t fact_index) const
    {
        assert(fact_index < size());
        return FactProxy(
            *task,
            task->get_operator_precondition(op_index, fact_index, is_axiom));
    }
};

class ProbabilisticEffectConditionsProxy
    : public ConditionsProxy<ProbabilisticEffectConditionsProxy> {
    int op_index;
    int outcome_index;
    int eff_index;
    bool is_axiom;

public:
    ProbabilisticEffectConditionsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index,
        bool is_axiom)
        : ConditionsProxy<ProbabilisticEffectConditionsProxy>(task)
        , op_index(op_index)
        , outcome_index(outcome_index)
        , eff_index(eff_index)
        , is_axiom(is_axiom)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_outcome_effect_conditions(
            op_index,
            outcome_index,
            eff_index,
            is_axiom);
    }

    FactProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return FactProxy(
            *task,
            task->get_operator_outcome_effect_condition(
                op_index,
                outcome_index,
                eff_index,
                index,
                is_axiom));
    }
};

class ProbabilisticEffectProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;
    int eff_index;
    bool is_axiom;

public:
    ProbabilisticEffectProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index,
        bool is_axiom)
        : task(&task)
        , op_index(op_index)
        , eff_index(eff_index)
        , is_axiom(is_axiom)
    {
    }

    ~ProbabilisticEffectProxy() = default;

    ProbabilisticEffectConditionsProxy get_conditions() const
    {
        return ProbabilisticEffectConditionsProxy(
            *task,
            op_index,
            outcome_index,
            eff_index,
            is_axiom);
    }

    FactProxy get_fact() const
    {
        return FactProxy(
            *task,
            task->get_operator_outcome_effect(
                op_index,
                outcome_index,
                eff_index,
                is_axiom));
    }
};

class ProbabilisticEffectsProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;
    bool is_axiom;

public:
    using ItemType = EffectProxy;
    ProbabilisticEffectsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        bool is_axiom)
        : task(&task)
        , op_index(op_index)
        , outcome_index(outcome_index)
        , is_axiom(is_axiom)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_outcome_effects(
            op_index,
            outcome_index,
            is_axiom);
    }

    ProbabilisticEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticEffectProxy(
            *task,
            op_index,
            outcome_index,
            eff_index,
            is_axiom);
    }
};

class ProbabilisticOutcomeProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;
    bool is_axiom;

public:
    ProbabilisticOutcomeProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        bool is_axiom)
        : task(&task)
        , op_index(op_index)
        , outcome_index(outcome_index)
        , is_axiom(is_axiom)
    {
    }

    ProbabilisticEffectsProxy get_effects() const
    {
        return ProbabilisticEffectsProxy(
            *task,
            op_index,
            outcome_index,
            is_axiom);
    }

    value_type::value_t get_probability() const
    {
        return task->get_operator_outcome_probability(
            op_index,
            outcome_index,
            is_axiom);
    }
};

class ProbabilisticOutcomesProxy {
    const ProbabilisticTask* task;
    int op_index;
    bool is_axiom;

public:
    using ItemType = ProbabilisticOutcomeProxy;
    ProbabilisticOutcomesProxy(
        const ProbabilisticTask& task,
        int op_index,
        bool is_axiom)
        : task(&task)
        , op_index(op_index)
        , is_axiom(is_axiom)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_outcomes(op_index, is_axiom);
    }

    ProbabilisticOutcomeProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticOutcomeProxy(*task, op_index, eff_index, is_axiom);
    }
};

class ProbabilisticOperatorProxy {
    const ProbabilisticTask* task;
    int index;
    bool is_an_axiom;

public:
    ProbabilisticOperatorProxy(
        const ProbabilisticTask& task,
        int index,
        bool is_axiom)
        : task(&task)
        , index(index)
        , is_an_axiom(is_axiom)
    {
    }

    bool operator==(const ProbabilisticOperatorProxy& other) const
    {
        assert(task == other.task);
        return index == other.index && is_an_axiom == other.is_an_axiom;
    }

    bool operator!=(const ProbabilisticOperatorProxy& other) const
    {
        return !(*this == other);
    }

    PreconditionsProxy get_preconditions() const
    {
        return PreconditionsProxy(*task, index, is_an_axiom);
    }

    ProbabilisticOutcomesProxy get_outcomes() const
    {
        return ProbabilisticOutcomesProxy(*task, index, is_an_axiom);
    }

    int get_cost() const { return task->get_operator_cost(index, is_an_axiom); }

    bool is_axiom() const { return is_an_axiom; }

    std::string get_name() const
    {
        return task->get_operator_name(index, is_an_axiom);
    }

    int get_id() const { return index; }

    /*
      Eventually, this method should perhaps not be part of OperatorProxy but
      live in a class that handles the task transformation and known about both
      the original and the transformed task.
    */
    OperatorID
    get_ancestor_operator_id(const ProbabilisticTask* ancestor_task) const
    {
        assert(!is_an_axiom);
        return OperatorID(task->convert_operator_index(index, ancestor_task));
    }
};

class ProbabilisticOperatorsProxy {
    const ProbabilisticTask* task;

public:
    using ItemType = OperatorProxy;
    explicit ProbabilisticOperatorsProxy(const ProbabilisticTask& task)
        : task(&task)
    {
    }

    std::size_t size() const { return task->get_num_operators(); }

    bool empty() const { return size() == 0; }

    ProbabilisticOperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return ProbabilisticOperatorProxy(*task, index, false);
    }

    ProbabilisticOperatorProxy operator[](OperatorID id) const
    {
        return (*this)[id.get_index()];
    }
};

class AxiomsProxy {
    const ProbabilisticTask* task;

public:
    using ItemType = OperatorProxy;
    explicit AxiomsProxy(const ProbabilisticTask& task)
        : task(&task)
    {
    }
    ~AxiomsProxy() = default;

    std::size_t size() const { return task->get_num_axioms(); }

    bool empty() const { return size() == 0; }

    ProbabilisticOperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return ProbabilisticOperatorProxy(*task, index, true);
    }
};

class TaskProxy {
    const ProbabilisticTask* task;

public:
    explicit TaskProxy(const ProbabilisticTask& task)
        : task(&task)
    {
    }
    ~TaskProxy() = default;

    TaskID get_id() const { return TaskID(task); }

    void subscribe_to_task_destruction(
        subscriber::Subscriber<ProbabilisticTask>* subscriber) const
    {
        task->subscribe(subscriber);
    }

    VariablesProxy get_variables() const { return VariablesProxy(*task); }

    ProbabilisticOperatorsProxy get_operators() const
    {
        return ProbabilisticOperatorsProxy(*task);
    }

    AxiomsProxy get_axioms() const { return AxiomsProxy(*task); }

    GoalsProxy get_goals() const { return GoalsProxy(*task); }

    /*State create_state(std::vector<int>&& state_values) const
    {
        return State(*task, std::move(state_values));
    }

    // This method is meant to be called only by the state registry.
    State create_state(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer) const
    {
        return State(*task, registry, id, buffer);
    }

    // This method is meant to be called only by the state registry.
    State create_state(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer,
        std::vector<int>&& state_values) const
    {
        return State(*task, registry, id, buffer, std::move(state_values));
    }

    State get_initial_state() const
    {
        return create_state(task->get_initial_state_values());
    }*/

    /*
      Convert a state from an ancestor task into a state of this task.
      The given state has to belong to a task that is an ancestor of
      this task in the sense that this task is the result of a sequence
      of task transformations on the ancestor task. If this is not the
      case, the function aborts.

      Eventually, this method should perhaps not be part of TaskProxy but live
      in a class that handles the task transformation and knows about both the
      original and the transformed task.
    */
    /*State convert_ancestor_state(const State& ancestor_state) const
    {
        TaskProxy ancestor_task_proxy = ancestor_state.get_task();
        // Create a copy of the state values for the new state.
        ancestor_state.unpack();
        std::vector<int> state_values = ancestor_state.get_unpacked_values();
        task->convert_ancestor_state_values(
            state_values,
            ancestor_task_proxy.task);
        return create_state(std::move(state_values));
    }*/

    const causal_graph::CausalGraph& get_causal_graph() const;
};

} // namespace probfd

#endif
