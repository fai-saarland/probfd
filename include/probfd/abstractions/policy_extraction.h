#ifndef PROBFD_ABSTRACTIONS_POLICY_EXTRACTION_H
#define PROBFD_ABSTRACTIONS_POLICY_EXTRACTION_H

#include "probfd/multi_policy.h"
#include "probfd/type_traits.h"
#include "probfd/value_type.h"

#include <memory>
#include <span>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd {
template <typename, typename>
class MDP;
}

namespace probfd {

/**
 * @brief Extract an abstract optimal policy from the value table.
 *
 * Tie-breaking is performed randomly using the input RNG. If the \p
 * wildcard option is specified, a wildcard policy will be returned, i.e., a
 * policy that assigns multiple equivalent operators to an abstract state.
 */
template <typename State, typename Action>
std::unique_ptr<MultiPolicy<State, Action>> compute_optimal_projection_policy(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    ParamType<State> initial_state,
    value_t greedy_epsilon,
    downward::utils::RandomNumberGenerator& rng,
    bool wildcard);

/**
 * @brief Extracts an abstract greedy policy from the value table, which may not
 * be optimal if traps are existent.
 *
 * Tie-breaking is performed randomly using the input RNG. If the \p
 * wildcard option is specified, a wildcard policy will be returned, i.e., a
 * policy that assigns multiple equivalent operators to an abstract state.
 */
template <typename State, typename Action>
std::unique_ptr<MultiPolicy<State, Action>> compute_greedy_projection_policy(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    ParamType<State> initial_state,
    value_t greedy_epsilon,
    downward::utils::RandomNumberGenerator& rng,
    bool wildcard);

} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ABSTRACTIONS_POLICY_EXTRACTION_H
#include "probfd/abstractions/policy_extraction_impl.h"
#undef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_POLICY_EXTRACTION_H

#endif
