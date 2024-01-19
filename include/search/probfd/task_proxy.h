#ifndef PROBFD_TASK_PROXY_H
#define PROBFD_TASK_PROXY_H

#include "probfd/probabilistic_task.h"

#include "downward/operator_id.h"
#include "downward/state_id.h"
#include "downward/task_id.h"

#include "downward/task_proxy.h"

#include "downward/utils/collections.h"
#include "downward/utils/hash.h"
#include "downward/utils/system.h"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

// Forward Declarations
namespace probfd {
class ProbabilisticEffectProxy;
class ProbabilisticEffectConditionsProxy;
class ProbabilisticEffectsProxy;
class ProbabilisticOperatorProxy;
class ProbabilisticOperatorsProxy;
class ProbabilisticOutcomeProxy;
class ProbabilisticOutcomesProxy;
class ProbabilisticTaskProxy;
} // namespace probfd

namespace probfd {

/// Proxy class used to inspect the list of effect conditions of a conditional
/// effect of a probabilistic operator. Can be used as a range of FactProxies,
/// one for each effect condition.
class ProbabilisticEffectConditionsProxy
    : public ProxyCollection<ProbabilisticEffectConditionsProxy> {
    const ProbabilisticTask* task_;

    int op_index_;
    int outcome_index_;
    int eff_index_;

public:
    ProbabilisticEffectConditionsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index)
        : task_(&task)
        , op_index_(op_index)
        , outcome_index_(outcome_index)
        , eff_index_(eff_index)
    {
    }

    /// Returns the number of effect conditions.
    [[nodiscard]]
    std::size_t size() const
    {
        return task_->get_num_operator_outcome_effect_conditions(
            op_index_,
            outcome_index_,
            eff_index_);
    }

    /// Accesses a specific effect condition by its list index.
    FactProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return FactProxy(
            *task_,
            task_->get_operator_outcome_effect_condition(
                op_index_,
                outcome_index_,
                eff_index_,
                static_cast<int>(index)));
    }
};

/// Proxy class used to inspect a probabilistic effect of a probabilistic
/// operator.
class ProbabilisticEffectProxy {
    const ProbabilisticTask* task_;
    int op_index_;
    int outcome_index_;
    int eff_index_;

public:
    ProbabilisticEffectProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index)
        : task_(&task)
        , op_index_(op_index)
        , outcome_index_(outcome_index)
        , eff_index_(eff_index)
    {
    }

    ~ProbabilisticEffectProxy() = default;

    /// Get a proxy for the conditions of the probabilistic effect.
    [[nodiscard]]
    ProbabilisticEffectConditionsProxy get_conditions() const
    {
        return ProbabilisticEffectConditionsProxy(
            *task_,
            op_index_,
            outcome_index_,
            eff_index_);
    }

    /// Get a proxy for the established fact of the probabilistic effect.
    [[nodiscard]]
    FactProxy get_fact() const
    {
        return FactProxy(
            *task_,
            task_->get_operator_outcome_effect(
                op_index_,
                outcome_index_,
                eff_index_));
    }
};

/// Proxy class used to inspect the list of probabilistic effects of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticEffectProxies, one for each probabilistic effect.
class ProbabilisticEffectsProxy
    : public ProxyCollection<ProbabilisticEffectsProxy> {
    const ProbabilisticTask* task_;
    int op_index_;
    int outcome_index_;

public:
    ProbabilisticEffectsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index)
        : task_(&task)
        , op_index_(op_index)
        , outcome_index_(outcome_index)
    {
    }

    /// Returns the number of probabilistic effects.
    [[nodiscard]]
    std::size_t size() const
    {
        return task_->get_num_operator_outcome_effects(
            op_index_,
            outcome_index_);
    }

    /// Accesses a specific probabilistic effect by its list index.
    ProbabilisticEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticEffectProxy(
            *task_,
            op_index_,
            outcome_index_,
            eff_index);
    }
};

/// Proxy class used to inspect a single probabilistic outcome of a
/// probabilistic operator.
class ProbabilisticOutcomeProxy {
    const ProbabilisticTask* task_;
    int op_index_;
    int outcome_index_;

public:
    ProbabilisticOutcomeProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index)
        : task_(&task)
        , op_index_(op_index)
        , outcome_index_(outcome_index)
    {
    }

    /// Get the ID of this outcome's operator in the determinization.
    [[nodiscard]]
    int get_determinization_id() const
    {
        return task_->get_operator_outcome_id(op_index_, outcome_index_);
    }

    /// Get this outcome's operator.
    [[nodiscard]]
    ProbabilisticOperatorProxy get_operator() const;

    /// Get a proxy to the probabilistic effects of this outcome.
    [[nodiscard]]
    ProbabilisticEffectsProxy get_effects() const
    {
        return ProbabilisticEffectsProxy(*task_, op_index_, outcome_index_);
    }

    /// Get the probability of this outcome.
    [[nodiscard]]
    value_t get_probability() const
    {
        return task_->get_operator_outcome_probability(
            op_index_,
            outcome_index_);
    }
};

/// Proxy class used to inspect the list of probabilistic outcomes of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticOutcomeProxies, one for each probabilistic outcome.
class ProbabilisticOutcomesProxy
    : public ProxyCollection<ProbabilisticOutcomesProxy> {
    const ProbabilisticTask* task_;
    int op_index_;

public:
    ProbabilisticOutcomesProxy(const ProbabilisticTask& task, int op_index)
        : task_(&task)
        , op_index_(op_index)
    {
    }

    /// Returns the number of outcomes of the probabilistic operator.
    [[nodiscard]]
    std::size_t size() const
    {
        return task_->get_num_operator_outcomes(op_index_);
    }

    /// Get the operator of the outcomes.
    [[nodiscard]]
    ProbabilisticOperatorProxy get_operator() const;

    /// Get a proxy to a specific outcome by its list index.
    ProbabilisticOutcomeProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return ProbabilisticOutcomeProxy(*task_, op_index_, eff_index);
    }
};

/// Proxy class used to inspect a single probabilistic operator.
class ProbabilisticOperatorProxy : public OperatorLightProxy {
public:
    ProbabilisticOperatorProxy(const ProbabilisticTask& task, int index)
        : OperatorLightProxy(task, index)
    {
    }

    /// Get a proxy to the outcomes of the operator.
    [[nodiscard]]
    ProbabilisticOutcomesProxy get_outcomes() const
    {
        return ProbabilisticOutcomesProxy(
            *static_cast<const ProbabilisticTask*>(task),
            index);
    }

    /// Get the cost of the operator.
    [[nodiscard]]
    value_t get_cost() const
    {
        return static_cast<const ProbabilisticTask*>(task)->get_operator_cost(
            index);
    }
};

inline ProbabilisticOperatorProxy
ProbabilisticOutcomeProxy::get_operator() const
{
    return ProbabilisticOperatorProxy(*task_, op_index_);
}

inline ProbabilisticOperatorProxy
ProbabilisticOutcomesProxy::get_operator() const
{
    return ProbabilisticOperatorProxy(*task_, op_index_);
}

/// Proxy class used to inspect a list of probabilistic operators of a
/// probabilistic task. Can be used as a range of ProbabilisticOperatorProxies,
/// one for each probabilistic operator.
class ProbabilisticOperatorsProxy
    : public ProxyCollection<ProbabilisticOperatorsProxy> {
    const ProbabilisticTask* task_;

public:
    explicit ProbabilisticOperatorsProxy(const ProbabilisticTask& task)
        : task_(&task)
    {
    }

    /// Returns the number of probabilistic operators in the list.
    [[nodiscard]]
    std::size_t size() const
    {
        return task_->get_num_operators();
    }

    /// Get a proxy for a specific probabilistic operator by list index.
    ProbabilisticOperatorProxy operator[](std::size_t index) const
    {
        assert(index < size());
        return ProbabilisticOperatorProxy(*task_, index);
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
    [[nodiscard]]
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
    return std::ranges::all_of(
        effect.get_conditions(),
        [&](FactProxy condition) {
            return state[condition.get_variable()] == condition;
        });
}

} // namespace probfd

#endif
