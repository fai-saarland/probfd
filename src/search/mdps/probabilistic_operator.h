#pragma once

#include "value_type.h"
#include "../global_operator.h"

#include <iterator>
#include <string>
#include <vector>

class GlobalOperator;
class GlobalCondition;
class GlobalState;

namespace probabilistic {

/** \struct ProbabilisticOutcome probabilistic_operator.h
 * @brief Struct representing a probabilistic outcome.
 * 
 * A probabilistic outcome has a probability, an effect and is additionally
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
};

/** \class ProbabilisticOperator probabilistic_operator.h
 * @brief Class representing a probabilistic operator.
 * 
 * Each probabilistic operator has
 * - A unique id
 * - A name
 * - A precondition
 * - A list of outcomes, each assigned a probability of occurence
 * - An integer cost
 */
class ProbabilisticOperator {
public:
    using const_iterator = std::vector<ProbabilisticOutcome>::const_iterator;

    ProbabilisticOperator(
        unsigned id,
        std::string name,
        std::vector<ProbabilisticOutcome> outcomes);

    /**
     * @brief Get the id of the operator
     * 
     * @return unsigned The id of the operator
     */
    unsigned get_id() const;

    /**
     * @brief Get the name of the operator
     * 
     * @return const std::string& The name of the operator
     */
    const std::string& get_name() const;

    /**
     * @return True if and only if the operator has more than two possible 
     * outcomes
     */
    bool is_stochastic() const;

    std::size_t num_outcomes() const;

    int get_cost() const;

    /**
     * @brief Queries a possible outcome of this operator by its index
     * 
     * @param i Index of the outcome
     */
    const ProbabilisticOutcome& operator[](unsigned i) const;

    /**
     * \copydoc ProbabilisticOperator::operator[]
     */
    const ProbabilisticOutcome& get(unsigned i) const;

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

extern bool is_applicable(const ProbabilisticOperator* op, const GlobalState& state);

} // namespace probabilistic
