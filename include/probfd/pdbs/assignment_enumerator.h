#ifndef PROBFD_PDBS_ASSIGNMENT_ENUMERATOR_H
#define PROBFD_PDBS_ASSIGNMENT_ENUMERATOR_H

#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

namespace probfd::pdbs {

class AssignmentEnumerator {
    struct VariableInfo {
        long long int multiplier;
        int domain;
    };

    std::vector<VariableInfo> var_infos_;
    long long int num_assignments_;

public:
    /**
     * @brief Constructs the enumeration function for empty assignments.
     */
    AssignmentEnumerator();

    /**
     * @brief Constructs the enumeration function for assignments with the
     * given domains.
     */
    explicit AssignmentEnumerator(const auto& domain_sizes)
        : var_infos_(domain_sizes.size())
    {
        using namespace std::views;
        constexpr auto maxint = std::numeric_limits<long long int>::max();

        long long int multiplier = 1;

        for (auto [cur_info, domain_size] : zip(var_infos_, domain_sizes)) {
            cur_info.domain = domain_size;
            cur_info.multiplier = multiplier;

            if (multiplier > maxint / domain_size) {
                throw std::range_error(
                    "Construction of PDB would exceed "
                    "std::numeric_limits<long long int>::max()");
            }

            multiplier *= domain_size;
        }

        num_assignments_ = multiplier;
    }

    /**
     * @brief Get the number of possible assignments.
     */
    [[nodiscard]]
    unsigned int num_assignments() const;

    /**
     * @brief Get the number of variables in the assignments.
     */
    [[nodiscard]]
    unsigned int num_vars() const;

    /**
     * @brief Get the multiplier coefficient
     * \f$ N_i = \prod_{j=0}^{i-1} |\mathcal{D}_j| \f$
     * for variable \f$ i \f$ of assignments.
     */
    [[nodiscard]]
    long long int get_multiplier(int var) const;

    /**
     * @brief Get the domain size of an assignment variable.
     */
    [[nodiscard]]
    int get_domain_size(int var) const;

    /**
     * @brief Ranks a fact by multiplying the ranking coefficient
     * of the fact's variable with the fact's value.
     *
     * Given the fact \f$(i, d)\f$, computes the value
     * \f$N_i \cdot d\f$ where \f$N_i = \prod_{j=0}^{i-1} |\mathcal{D}_j|\f$
     * is the multiplier of variable
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
    std::vector<int> unrank(int assignment_index) const;

    /**
     * @brief Compute the value of a variable for the given assignment index.
     *
     * In detail, computes the value
     * \f[
     * unrank(r)[v_i] = \lfloor \frac{r}{N_i} \rfloor \mod
     * \mathcal{D}_i.
     * \f]
     */
    [[nodiscard]]
    int value_of(int assignment_index, int idx) const;

    /**
     * @brief Compute the next-highest index that corresponds to the same
     * assignment modulo a given subset of variable values.
     *
     * In detail, let \f$r\f$ be the input index and let
     * \f$\{i_1, \dots, i_m\}\f$
     * be the subset of assignment variables. Define
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
     * Then the returned index is
     * \f$r' = \sum_{i=0}^{k} N_i d_i'\f$.
     *
     * @returns false iff the index is already maximal, in which case the lowest
     * index is returned.
     */
    bool
    next_index(int& assignment_index, std::span<int> mutable_variables) const;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_ASSIGNMENT_ENUMERATOR_H
