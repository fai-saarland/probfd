#ifndef PROBFD_PROBABILISTIC_OPERATOR_SPACE_H
#define PROBFD_PROBABILISTIC_OPERATOR_SPACE_H

#include "downward/operator_space.h" // IWYU pragma: export
#include "downward/proxy_collection.h"

#include "probfd/value_type.h"

namespace downward {
class State;
class AxiomEvaluator;
class OperatorPreconditionsProxy;
}

namespace probfd {

class ProbabilisticOperatorProxy;
class ProbabilisticOperatorsProxy;

/**
 * @brief Represents a probabilistic planning op_space with axioms and
 * conditional effects.
 *
 * This class should not be used directly to access the information of a
 * probabilistic planning op_space. To this end, use
 * ProbabilisticOperatorSpaceProxy.
 *
 * @see ProbabilisticOperatorSpaceProxy
 */
class ProbabilisticOperatorSpace : public virtual downward::OperatorSpace {
public:
    /// Get the number of probabilistic outcomes of the probabilistic operator
    /// with index \p op_index.
    virtual int get_num_operator_outcomes(int op_index) const = 0;

    /// Get the probabilistic outcome probability of the outcome with index
    /// \p outcome_index of the probabilistic operator with index \p op_index.
    virtual value_t
    get_operator_outcome_probability(int op_index, int outcome_index) const = 0;

    /// Get the global outcome index for the outcome with index \p outcome_index
    /// of the probabilistic operator with index \p op_index.
    virtual int
    get_operator_outcome_id(int op_index, int outcome_index) const = 0;

    /// Get the number of effects of the outcome with index \p outcome_index of
    /// the probabilistic operator with index \p op_index.
    virtual int
    get_num_operator_outcome_effects(int op_index, int outcome_index) const = 0;

    /// Get the effect with index \p eff_index of the outcome with index
    /// \p outcome_index of the probabilistic operator with index \p op_index.
    virtual downward::FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const = 0;

    /// Get the number of effect conditions for the effect with index
    /// \p eff_index of the outcome with index \p outcome_index of the
    /// probabilistic operator with index \p op_index.
    virtual int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const = 0;

    /// Get the number of effect conditions for the effects with given index of
    /// outcome with given index of the probabilistic operator with given index.
    virtual downward::FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const = 0;

    /// Returns a proxy for the list of probabilistic operators.
    [[nodiscard]]
    ProbabilisticOperatorsProxy get_operators() const;
};

/// Proxy class used to inspect the list of effect conditions of a conditional
/// effect of a probabilistic operator. Can be used as a range of FactProxies,
/// one for each effect condition.
class ProbabilisticEffectConditionsProxy
    : public downward::ProxyCollection<ProbabilisticEffectConditionsProxy> {
    const ProbabilisticOperatorSpace* op_space_;

    int op_index_;
    int outcome_index_;
    int eff_index_;

public:
    ProbabilisticEffectConditionsProxy(
        const ProbabilisticOperatorSpace& op_space,
        int op_index,
        int outcome_index,
        int eff_index);

    /// Returns the number of effect conditions.
    [[nodiscard]]
    std::size_t size() const;

    /// Accesses a specific effect condition by its list index.
    downward::FactPair operator[](std::size_t index) const;
};

/// Proxy class used to inspect a probabilistic effect of a probabilistic
/// operator.
class ProbabilisticEffectProxy {
    const ProbabilisticOperatorSpace* op_space_;
    int op_index_;
    int outcome_index_;
    int eff_index_;

public:
    ProbabilisticEffectProxy(
        const ProbabilisticOperatorSpace& op_space,
        int op_index,
        int outcome_index,
        int eff_index);

    /// Get a proxy for the conditions of the probabilistic effect.
    [[nodiscard]]
    ProbabilisticEffectConditionsProxy get_conditions() const;

    /// Get a proxy for the established fact of the probabilistic effect.
    [[nodiscard]]
    downward::FactPair get_fact() const;
};

/// Proxy class used to inspect the list of probabilistic effects of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticEffectProxies, one for each probabilistic effect.
class ProbabilisticEffectsProxy
    : public downward::ProxyCollection<ProbabilisticEffectsProxy> {
    const ProbabilisticOperatorSpace* op_space_;
    int op_index_;
    int outcome_index_;

public:
    ProbabilisticEffectsProxy(
        const ProbabilisticOperatorSpace& op_space,
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
    const ProbabilisticOperatorSpace* op_space_;
    int op_index_;
    int outcome_index_;

public:
    ProbabilisticOutcomeProxy(
        const ProbabilisticOperatorSpace& op_space,
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
    downward::State get_unregistered_successor(
        const downward::State& state,
        downward::AxiomEvaluator& axiom_evaluator) const;
};

/// Proxy class used to inspect the list of probabilistic outcomes of a
/// probabilistic operator. Can be used as a range of
/// ProbabilisticOutcomeProxies, one for each probabilistic outcome.
class ProbabilisticOutcomesProxy
    : public downward::ProxyCollection<ProbabilisticOutcomesProxy> {
    const ProbabilisticOperatorSpace* op_space_;
    int op_index_;

public:
    ProbabilisticOutcomesProxy(
        const ProbabilisticOperatorSpace& op_space,
        int op_index);

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
class ProbabilisticOperatorProxy {
    const ProbabilisticOperatorSpace* op_space_;
    int index_;

public:
    ProbabilisticOperatorProxy(
        const ProbabilisticOperatorSpace& op_space,
        int index);

    operator downward::PartialOperatorProxy() const
    {
        return  downward::PartialOperatorProxy(*op_space_, index_);
    }

    downward::OperatorPreconditionsProxy get_preconditions() const;

    /// Get a proxy to the outcomes of the operator.
    [[nodiscard]]
    ProbabilisticOutcomesProxy get_outcomes() const;

    std::string get_name() const;

    int get_id() const;

    friend bool operator==(
        const ProbabilisticOperatorProxy& left,
        const ProbabilisticOperatorProxy& right);
};

/// Proxy class used to inspect a list of probabilistic operators of a
/// probabilistic op_space. Can be used as a range of
/// ProbabilisticOperatorProxies, one for each probabilistic operator.
class ProbabilisticOperatorsProxy
    : public downward::ProxyCollection<ProbabilisticOperatorsProxy> {
    const ProbabilisticOperatorSpace* op_space_;

public:
    explicit ProbabilisticOperatorsProxy(
        const ProbabilisticOperatorSpace& op_space);

    /// Returns the number of probabilistic operators in the list.
    [[nodiscard]]
    std::size_t size() const;

    /// Get a proxy for a specific probabilistic operator by list index.
    ProbabilisticOperatorProxy operator[](std::size_t index) const;

    /// Get a proxy for a specific probabilistic operator by operator id.
    ProbabilisticOperatorProxy operator[](downward::OperatorID id) const;
};

} // namespace probfd

#endif