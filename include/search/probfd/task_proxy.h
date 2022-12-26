#ifndef PROBFD_TASK_PROXY_H
#define PROBFD_TASK_PROXY_H

#include "probfd/probabilistic_task.h"

#include "operator_id.h"
#include "state_id.h"
#include "task_id.h"

#include "../task_proxy.h"

#include "utils/collections.h"
#include "utils/hash.h"
#include "utils/system.h"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

namespace probfd {

class ProbabilisticEffectProxy;
class ProbabilisticEffectConditionsProxy;
class ProbabilisticEffectsProxy;
class ProbabilisticOperatorProxy;
class ProbabilisticOperatorsProxy;
class ProbabilisticOutcomeProxy;
class ProbabilisticOutcomesProxy;
class ProbabilisticTaskProxy;

/*
 * Proxy Iterator suppport
 */
template <class ProxyCollection>
inline ProxyIterator<ProxyCollection> begin(ProxyCollection& collection)
{
    return ProxyIterator<ProxyCollection>(collection, 0);
}

template <class ProxyCollection>
inline ProxyIterator<ProxyCollection> end(ProxyCollection& collection)
{
    return ProxyIterator<ProxyCollection>(collection, collection.size());
}

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

class ProbabilisticEffectConditionsProxy
    : public ConditionsProxy<ProbabilisticEffectConditionsProxy> {
    int op_index;
    int outcome_index;
    int eff_index;

public:
    ProbabilisticEffectConditionsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index)
        : ConditionsProxy<ProbabilisticEffectConditionsProxy>(task)
        , op_index(op_index)
        , outcome_index(outcome_index)
        , eff_index(eff_index)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_outcome_effect_conditions(
            op_index,
            outcome_index,
            eff_index);
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
                index));
    }
};

class ProbabilisticEffectProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;
    int eff_index;

public:
    ProbabilisticEffectProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index)
        : task(&task)
        , op_index(op_index)
        , outcome_index(outcome_index)
        , eff_index(eff_index)
    {
    }

    ~ProbabilisticEffectProxy() = default;

    ProbabilisticEffectConditionsProxy get_conditions() const
    {
        return ProbabilisticEffectConditionsProxy(
            *task,
            op_index,
            outcome_index,
            eff_index);
    }

    FactProxy get_fact() const
    {
        return FactProxy(
            *task,
            task->get_operator_outcome_effect(
                op_index,
                outcome_index,
                eff_index));
    }
};

class ProbabilisticEffectsProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;

public:
    using ItemType = ProbabilisticEffectProxy;
    ProbabilisticEffectsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index)
        : task(&task)
        , op_index(op_index)
        , outcome_index(outcome_index)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_outcome_effects(op_index, outcome_index);
    }

    ProbabilisticEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticEffectProxy(
            *task,
            op_index,
            outcome_index,
            eff_index);
    }
};

class ProbabilisticOutcomeProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;

public:
    ProbabilisticOutcomeProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index)
        : task(&task)
        , op_index(op_index)
        , outcome_index(outcome_index)
    {
    }

    ProbabilisticEffectsProxy get_effects() const
    {
        return ProbabilisticEffectsProxy(*task, op_index, outcome_index);
    }

    value_type::value_t get_probability() const
    {
        return task->get_operator_outcome_probability(op_index, outcome_index);
    }
};

class ProbabilisticOutcomesProxy {
    const ProbabilisticTask* task;
    int op_index;

public:
    using ItemType = ProbabilisticOutcomeProxy;
    ProbabilisticOutcomesProxy(const ProbabilisticTask& task, int op_index)
        : task(&task)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return task->get_num_operator_outcomes(op_index);
    }

    ProbabilisticOutcomeProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticOutcomeProxy(*task, op_index, eff_index);
    }
};

class ProbabilisticOperatorProxy {
    const ProbabilisticTask* task;
    int index;

public:
    ProbabilisticOperatorProxy(const ProbabilisticTask& task, int index)
        : task(&task)
        , index(index)
    {
    }

    bool operator==(const ProbabilisticOperatorProxy& other) const
    {
        assert(task == other.task);
        return index == other.index;
    }

    bool operator!=(const ProbabilisticOperatorProxy& other) const
    {
        return !(*this == other);
    }

    OperatorPreconditionsProxy get_preconditions() const
    {
        return OperatorPreconditionsProxy(*task, index);
    }

    ProbabilisticOutcomesProxy get_outcomes() const
    {
        return ProbabilisticOutcomesProxy(*task, index);
    }

    int get_reward() const { return task->get_operator_reward(index); }

    std::string get_name() const { return task->get_operator_name(index); }

    int get_id() const { return index; }

    /*
      Eventually, this method should perhaps not be part of OperatorProxy but
      live in a class that handles the task transformation and known about both
      the original and the transformed task.
    */
    OperatorID
    get_ancestor_operator_id(const ProbabilisticTask* ancestor_task) const
    {
        return OperatorID(task->convert_operator_index(index, ancestor_task));
    }
};

class ProbabilisticOperatorsProxy {
    const ProbabilisticTask* task;

public:
    using ItemType = ProbabilisticOperatorProxy;
    explicit ProbabilisticOperatorsProxy(const ProbabilisticTask& task)
        : task(&task)
    {
    }

    std::size_t size() const { return task->get_num_operators(); }

    bool empty() const { return size() == 0; }

    ProbabilisticOperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return ProbabilisticOperatorProxy(*task, index);
    }

    ProbabilisticOperatorProxy operator[](OperatorID id) const
    {
        return (*this)[id.get_index()];
    }
};

class ProbabilisticTaskProxy : public TaskBaseProxy {
public:
    explicit ProbabilisticTaskProxy(const ProbabilisticTask& task)
        : TaskBaseProxy(task)
    {
    }

    ProbabilisticOperatorsProxy get_operators() const
    {
        return ProbabilisticOperatorsProxy(
            *static_cast<const ProbabilisticTask*>(task));
    }
};

} // namespace probfd

#endif
