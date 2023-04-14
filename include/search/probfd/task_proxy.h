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

/// Returns the begin iterator of a proxy collection.
template <class ProxyCollection>
inline ProxyIterator<ProxyCollection> begin(ProxyCollection& collection)
{
    return ProxyIterator<ProxyCollection>(collection, 0);
}

/// Returns the end iterator for a proxy collection.
template <class ProxyCollection>
inline ProxyIterator<ProxyCollection> end(ProxyCollection& collection)
{
    return ProxyIterator<ProxyCollection>(collection, collection.size());
}

/// Proxy class used to inspect the list of effect conditions of a conditional
/// effect of a probabilistic operator. Can be used as a range of FactProxies,
/// one for each effect condition.
class ProbabilisticEffectConditionsProxy {
    const ProbabilisticTask* task;

    int op_index;
    int outcome_index;
    int eff_index;

public:
    /// The item type for ProxyIterator to support usage as a range.
    using ItemType = FactProxy;

    ProbabilisticEffectConditionsProxy(
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

    /// Returns the number of effect conditions.
    std::size_t size() const
    {
        return task->get_num_operator_outcome_effect_conditions(
            op_index,
            outcome_index,
            eff_index);
    }

    /// Accesses a specific effect condition by its list index.
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

    /// Checks if the list of effect conditions is empty.
    bool empty() const { return size() == 0; }
};

/// Proxy class used to inspect a probabilistic effect of a probabilistic
/// operator.
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

    /// Get a proxy for the conditions of the probabilistic effect.
    ProbabilisticEffectConditionsProxy get_conditions() const
    {
        return ProbabilisticEffectConditionsProxy(
            *task,
            op_index,
            outcome_index,
            eff_index);
    }

    /// Get a proxy for the established fact of the probabilistic effect.
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

/// Proxy class used to inspect the list of probabilistic effects of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticEffectProxies, one for each probabilistic effect.
class ProbabilisticEffectsProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;

public:
    /// The item type for ProxyIterator to support usage as a range.
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

    /// Returns the number of probabilistic effects.
    std::size_t size() const
    {
        return task->get_num_operator_outcome_effects(op_index, outcome_index);
    }

    /// Accesses a specific probabilistic effect by its list index.
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

/// Proxy class used to inspect a single probabilistic outcome of a
/// probabilistic operator.
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

    /// Get the ID of this outcome's operator in the determinization.
    int get_determinization_id() const
    {
        return task->get_operator_outcome_id(op_index, outcome_index);
    }

    /// Get a proxy to the probabilistic effects of this outcome.
    ProbabilisticEffectsProxy get_effects() const
    {
        return ProbabilisticEffectsProxy(*task, op_index, outcome_index);
    }

    /// Get the probability of this outcome.
    value_t get_probability() const
    {
        return task->get_operator_outcome_probability(op_index, outcome_index);
    }
};

/// Proxy class used to inspect the list of probabilistic outcomes of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticOutcomeProxies, one for each probabilistic outcome.
class ProbabilisticOutcomesProxy {
    const ProbabilisticTask* task;
    int op_index;

public:
    /// The item type for ProxyIterator to support usage as a range.
    using ItemType = ProbabilisticOutcomeProxy;

    ProbabilisticOutcomesProxy(const ProbabilisticTask& task, int op_index)
        : task(&task)
        , op_index(op_index)
    {
    }

    /// Returns the number of outcomes of the probabilistic operator.
    std::size_t size() const
    {
        return task->get_num_operator_outcomes(op_index);
    }

    /// Get a proxy to a specific outcome by its list index.
    ProbabilisticOutcomeProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticOutcomeProxy(*task, op_index, eff_index);
    }
};

/// Proxy class used to inspect a single probabilistic operator.
class ProbabilisticOperatorProxy {
    const ProbabilisticTask* task;
    int index;

public:
    ProbabilisticOperatorProxy(const ProbabilisticTask& task, int index)
        : task(&task)
        , index(index)
    {
    }

    /// Checks if two proxies reference the same probabilistic operator.
    bool operator==(const ProbabilisticOperatorProxy& other) const
    {
        assert(task == other.task);
        return index == other.index;
    }

    /// Checks if two proxies reference the same probabilistic operator.
    bool operator!=(const ProbabilisticOperatorProxy& other) const
    {
        return !(*this == other);
    }

    /// Get a proxy to the preconditions of the operator.
    OperatorPreconditionsProxy get_preconditions() const
    {
        return OperatorPreconditionsProxy(*task, index);
    }

    /// Get a proxy to the outcomes of the operator.
    ProbabilisticOutcomesProxy get_outcomes() const
    {
        return ProbabilisticOutcomesProxy(*task, index);
    }

    /// Get the cost of the operator.
    value_t get_cost() const { return task->get_operator_cost(index); }

    /// Get the name of the operator.
    std::string get_name() const { return task->get_operator_name(index); }

    /// Get the operator id of the operator.
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

/// Proxy class used to inspect a list of probabilistic operators of a
/// probabilistic task. Can be used as a range of ProbabilisticOperatorProxies,
/// one for each probabilistic operator.
class ProbabilisticOperatorsProxy {
    const ProbabilisticTask* task;

public:
    /// The item type for ProxyIterator to support usage as a range.
    using ItemType = ProbabilisticOperatorProxy;

    explicit ProbabilisticOperatorsProxy(const ProbabilisticTask& task)
        : task(&task)
    {
    }

    /// Returns the number of probabilistic operators in the list.
    std::size_t size() const { return task->get_num_operators(); }

    /// Checks if the list of operators is empty.
    bool empty() const { return size() == 0; }

    /// Get a proxy for a specific probabilistic operator by list index.
    ProbabilisticOperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return ProbabilisticOperatorProxy(*task, index);
    }

    /// Get a proxy for a specific probabilistic operator by operator id.
    ProbabilisticOperatorProxy operator[](OperatorID id) const
    {
        return (*this)[id.get_index()];
    }
};

/// Proxy class used to inspect a probabilistic planning task.
class ProbabilisticTaskProxy : public TaskBaseProxy {
public:
    explicit ProbabilisticTaskProxy(const ProbabilisticTask& task)
        : TaskBaseProxy(task)
    {
    }

    /// Returns a proxy for the list of probabilistic operators.
    ProbabilisticOperatorsProxy get_operators() const
    {
        return ProbabilisticOperatorsProxy(
            *static_cast<const ProbabilisticTask*>(task));
    }
};

/// Checks if the conditions of a probabilistic effect are fulfilled in a state.
inline bool
does_fire(const ProbabilisticEffectProxy& effect, const State& state)
{
    for (FactProxy condition : effect.get_conditions()) {
        if (state[condition.get_variable()] != condition) return false;
    }
    return true;
}

} // namespace probfd

#endif
