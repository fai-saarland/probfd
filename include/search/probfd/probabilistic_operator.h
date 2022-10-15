#ifndef MDPS_PROBABILISTIC_OPERATOR_H
#define MDPS_PROBABILISTIC_OPERATOR_H

#include "probfd/value_type.h"

#include "global_operator.h"

#include <iterator>
#include <string>
#include <vector>

class GlobalOperator;
struct GlobalCondition;
class GlobalState;

namespace probfd {

/**
 * @brief Struct representing a probabilistic outcome.
 *
 * A probabilistic outcome has a probability, an effect and additionally
 * stores a reference to the deterministic action induced by its all-outcomes
 * determinization. The effect is not stored explicitly, but can be accessed
 * via the this operator instead.
 */
struct ProbabilisticOutcome {
    const GlobalOperator* op; ///< The all-outcomes determinization
    value_type::value_t prob; ///< The probability of occurence

    explicit ProbabilisticOutcome(
        const GlobalOperator* op,
        value_type::value_t prob);

    const auto& effects() { return op->get_effects(); }
    const auto& effects() const { return op->get_effects(); }
};

/**
 * @brief Class representing a probabilistic operator.
 *
 * Each probabilistic operator has
 * - A unique id
 * - A name
 * - A precondition
 * - A list of outcomes, each assigned a probability of occurence
 * - An integer reward
 */
class ProbabilisticOperator {
public:
    using const_iterator = std::vector<ProbabilisticOutcome>::const_iterator;

    ProbabilisticOperator(
        unsigned id,
        std::string name,
        std::vector<ProbabilisticOutcome> outcomes);

    /**
     * @brief Gets the id of the operator
     */
    unsigned get_id() const;

    /**
     * @brief Gets the name of the operator
     */
    const std::string& get_name() const;

    /**
     * @brief Checks if the operator has at least two outcomes.
     */
    bool is_stochastic() const;

    /**
     * @brief Returns the number of outcomes of this operator.
     */
    std::size_t num_outcomes() const;

    /**
     * @brief Gets the reward of the operator.
     */
    int get_reward() const;

    /**
     * @brief Accesses a possible outcome of this operator by its index.
     */
    const ProbabilisticOutcome& operator[](unsigned i) const;

    /**
     * \copydoc ProbabilisticOperator::operator[]
     */
    const ProbabilisticOutcome& get(unsigned i) const;

    /**
     * @brief Gets the precondition of this operator.
     */
    const std::vector<GlobalCondition>& get_preconditions() const;

    /**
     * @brief Returns an iterator for the beginning of the outcome sequence
     */
    const_iterator begin() const;

    /**
     * @brief Returns an end iterator for the outcome sequence
     */
    const_iterator end() const;

private:
    unsigned id_;
    std::vector<ProbabilisticOutcome> outcomes_;
    std::string name_;
};

/**
 * @brief Checks if a probabilistic operator is applicable in a global state,
 * i.e. the precondition is satisfied.
 *
 * \todo make friend function
 */
extern bool
is_applicable(const ProbabilisticOperator* op, const GlobalState& state);

} // namespace probfd

#endif // __PROBABILISTIC_OPERATOR_H__