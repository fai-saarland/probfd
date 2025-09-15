#ifndef PROBFD_PDBS_STATE_RANKING_FUNCTION_H
#define PROBFD_PDBS_STATE_RANKING_FUNCTION_H

#include "probfd/pdbs/assignment_enumerator.h"
#include "probfd/pdbs/types.h"

#include "downward/state.h"

#include <span>
#include <string>
#include <vector>

namespace probfd::pdbs {

/**
 * @brief Implements the state ranking function for abstract states of
 * projections.
 *
 * A state ranking function is a bijective mapping from states to so-called
 * state ranks, which are indices in \f$\{ 0, \dots, N-1 \}\f$, where \f$N\f$
 * is the total number of states. This class implements the traditional PDB
 * (un-) ranking functions as stated in \cite sievers:etal:socs-12 .
 */
class StateRankingFunction {
    Pattern pattern_;
    AssignmentEnumerator enumerator_;

public:
    StateRankingFunction() = default;

    /**
     * @brief Constructs the ranking function for a projection specified by a
     * given pattern and the task's variables.
     */
    StateRankingFunction(
        const downward::VariableSpace& variables,
        Pattern pattern);

    /**
     * @brief Get the number of abstract states.
     */
    [[nodiscard]]
    unsigned int num_states() const;

    /**
     * @brief Get the number of variables considered by the projection.
     */
    [[nodiscard]]
    unsigned int num_vars() const;

    /**
     * @brief Get the pattern of the projection.
     */
    [[nodiscard]]
    const Pattern& get_pattern() const;

    /**
     * @brief Get the assignment enumerator for the projection's assignments.
     */
    [[nodiscard]]
    const AssignmentEnumerator& get_enumerator() const;

    /**
     * @brief Get the ranking coefficient
     * \f$ N_i = \prod_{j=0}^{i-1} |\mathcal{D}_j| \f$
     * for variable \f$ i \f$ of the projection.
     */
    [[nodiscard]]
    long long int get_multiplier(int i) const;

    /**
     * @brief Get the domain size for a projection variable.
     */
    [[nodiscard]]
    int get_domain_size(int i) const;

    /**
     * @brief Get the rank of the abstract state induced by a state.
     *
     * Computes the value
     * \f[ rank(s) = \sum_{i=0}^{k} N_i s[v_i] \f]
     * where
     * \f$N_i = \prod_{j=0}^{i-1} |\mathcal{D}_j|\f$
     * is the ranking coefficient of projection variable
     * \f$i \in \{1, \dots, k\}\f$.
     */
    [[nodiscard]]
    StateRank get_abstract_rank(const downward::State& state) const;

    /**
     * @brief Ranks a projection fact by multiplying the ranking coefficient
     * of fact's variable with the fact's value.
     *
     * Given the projection fact \f$(i, d)\f$, computes the value
     * \f$N_i \cdot d\f$ where \f$N_i = \prod_{j=0}^{i-1} |\mathcal{D}_j|\f$
     * is the ranking coefficient of projection variable
     * \f$i \in \{1, \dots, k\}\f$.
     */
    [[nodiscard]]
    int rank_fact(int idx, int val) const;

    /**
     * @brief Unrank a given state rank and converts it into an explicit
     * abstract state.
     *
     * The unranked abstract state \f$unrank(r)\f$ for the rank \f$r\f$ is
     * computes as
     * \f[
     * unrank(r)[v_i] = \lfloor \frac{r}{N_i} \rfloor \mod
     * \mathcal{D}_i
     * \f]
     * where
     * \f$N_i = \prod_{j=0}^{i-1} |\mathcal{D}_j|\f$
     * is the ranking coefficient of projection variable
     * \f$i \in \{1, \dots, k\}\f$.
     */
    [[nodiscard]]
    std::vector<int> unrank(StateRank abstract_state) const;

    /**
     * @brief Compute the value of a projection variable for the abstract state
     * of a state rank.
     *
     * In detail, computes the value
     * \f[
     * unrank(r)[v_i] = \lfloor \frac{r}{N_i} \rfloor \mod
     * \mathcal{D}_i.
     * \f]
     */
    [[nodiscard]]
    int value_of(StateRank rank, int idx) const;

    /**
     * @brief Compute the next-highest rank that corresponds to the same
     * abstract state modulo a given subset of projection variable values.
     *
     * In detail, let \f$r\f$ be the input rank and let
     * \f$\{i_1, \dots, i_m\}\f$
     * be the subset of projection variables. Define
     * \f[
     *   d_{i_j} =
     *   \begin{cases}
     *   unrank(r)[v_{i_j}] + 1 &
     *     unrank(r)[v_{i_j}] \neq \mathcal{D}_{i_j} - 1,\\
     *   0 &
     *     unrank(r)[v_{i_j}] = \mathcal{D}_{i_j} - 1.
     *   \end{cases}
     * \f]
     * for \f$i \in \{1, \dots, m\}\f$.
     * Then the returned rank is
     * \f$r' = \sum_{i=0}^{k} N_i d_i'\f$.
     *
     * @returns false iff the rank is already maximal, in which case the lowest
     * rank is returned.
     */
    bool next_rank(StateRank& s, std::span<int> mutable_variables) const;
};

class StateRankToString {
    const downward::VariableSpace& variables_;
    const StateRankingFunction& state_mapper_;

public:
    explicit StateRankToString(
        const downward::VariableSpace& variables,
        const StateRankingFunction& state_mapper);

    std::string operator()(StateRank state) const;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_STATE_RANKING_FUNCTION_H
