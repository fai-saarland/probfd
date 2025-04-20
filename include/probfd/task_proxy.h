#ifndef PROBFD_TASK_PROXY_H
#define PROBFD_TASK_PROXY_H

#include "probfd/value_type.h"

#include "downward/operator_id.h"

#include "downward/task_proxy.h"

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
class ProbabilisticTask;
} // namespace probfd

namespace probfd::causal_graph {
class ProbabilisticCausalGraph;
}

namespace probfd {

/// Proxy class used to inspect the list of effect conditions of a conditional
/// effect of a probabilistic operator. Can be used as a range of FactProxies,
/// one for each effect condition.
class ProbabilisticEffectConditionsProxy
    : public downward::ProxyCollection<ProbabilisticEffectConditionsProxy> {
    const ProbabilisticTask* task_;

    int op_index_;
    int outcome_index_;
    int eff_index_;

public:
    ProbabilisticEffectConditionsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index,
        int eff_index);

    /// Returns the number of effect conditions.
    [[nodiscard]]
    std::size_t size() const;

    /// Accesses a specific effect condition by its list index.
    downward::FactProxy operator[](std::size_t index) const;
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
        int eff_index);

    /// Get a proxy for the conditions of the probabilistic effect.
    [[nodiscard]]
    ProbabilisticEffectConditionsProxy get_conditions() const;

    /// Get a proxy for the established fact of the probabilistic effect.
    [[nodiscard]]
    downward::FactProxy get_fact() const;
};

/// Proxy class used to inspect the list of probabilistic effects of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticEffectProxies, one for each probabilistic effect.
class ProbabilisticEffectsProxy
    : public downward::ProxyCollection<ProbabilisticEffectsProxy> {
    const ProbabilisticTask* task_;
    int op_index_;
    int outcome_index_;

public:
    ProbabilisticEffectsProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index);

    /// Returns the number of probabilistic effects.
    [[nodiscard]]
    std::size_t size() const;

    /// Accesses a specific probabilistic effect by its list index.
    ProbabilisticEffectProxy operator[](std::size_t eff_index) const;
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
        int outcome_index);

    /// Get the ID of this outcome's operator in the determinization.
    [[nodiscard]]
    int get_determinization_id() const;

    /// Get this outcome's operator.
    [[nodiscard]]
    ProbabilisticOperatorProxy get_operator() const;

    /// Get a proxy to the probabilistic effects of this outcome.
    [[nodiscard]]
    ProbabilisticEffectsProxy get_effects() const;

    /// Get the probability of this outcome.
    [[nodiscard]]
    value_t get_probability() const;

    /// Get the successor state for this outcome.
    [[nodiscard]]
    downward::State
    get_unregistered_successor(const downward::State& state) const;
};

/// Proxy class used to inspect the list of probabilistic outcomes of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticOutcomeProxies, one for each probabilistic outcome.
class ProbabilisticOutcomesProxy
    : public downward::ProxyCollection<ProbabilisticOutcomesProxy> {
    const ProbabilisticTask* task_;
    int op_index_;

public:
    ProbabilisticOutcomesProxy(const ProbabilisticTask& task, int op_index);

    /// Get the operator of the outcomes.
    [[nodiscard]]
    ProbabilisticOperatorProxy get_operator() const;

    /// Returns the number of outcomes of the probabilistic operator.
    [[nodiscard]]
    std::size_t size() const;

    /// Get a proxy to a specific outcome by its list index.
    ProbabilisticOutcomeProxy operator[](std::size_t eff_index) const;
};

/// Proxy class used to inspect a single probabilistic operator.
class ProbabilisticOperatorProxy : public downward::PartialOperatorProxy {
public:
    ProbabilisticOperatorProxy(const ProbabilisticTask& task, int index);

    /// Get a proxy to the outcomes of the operator.
    [[nodiscard]]
    ProbabilisticOutcomesProxy get_outcomes() const;

    /// Get the cost of the operator.
    [[nodiscard]]
    value_t get_cost() const;
};

/// Proxy class used to inspect a list of probabilistic operators of a
/// probabilistic task. Can be used as a range of ProbabilisticOperatorProxies,
/// one for each probabilistic operator.
class ProbabilisticOperatorsProxy
    : public downward::ProxyCollection<ProbabilisticOperatorsProxy> {
    const ProbabilisticTask* task_;

public:
    explicit ProbabilisticOperatorsProxy(const ProbabilisticTask& task);

    /// Returns the number of probabilistic operators in the list.
    [[nodiscard]]
    std::size_t size() const;

    /// Get a proxy for a specific probabilistic operator by list index.
    ProbabilisticOperatorProxy operator[](std::size_t index) const;

    /// Get a proxy for a specific probabilistic operator by operator id.
    ProbabilisticOperatorProxy operator[](downward::OperatorID id) const;
};

/// Proxy class used to inspect a probabilistic planning task.
class ProbabilisticTaskProxy : public downward::PlanningTaskProxy {
public:
    explicit ProbabilisticTaskProxy(const ProbabilisticTask& task);

    /// Returns a proxy for the list of probabilistic operators.
    [[nodiscard]]
    ProbabilisticOperatorsProxy get_operators() const;

    const causal_graph::ProbabilisticCausalGraph& get_causal_graph() const;
};

/// Checks if the conditions of a probabilistic effect are fulfilled in a state.
bool does_fire(
    const ProbabilisticEffectProxy& effect,
    const downward::State& state);

static_assert(downward::OperatorLike<ProbabilisticOutcomeProxy>);

} // namespace probfd

#endif
