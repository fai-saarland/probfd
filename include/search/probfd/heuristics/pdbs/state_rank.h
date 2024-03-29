#ifndef PROBFD_HEURISTICS_PDBS_ABSTRACT_STATE_H
#define PROBFD_HEURISTICS_PDBS_ABSTRACT_STATE_H

#include "probfd/type_traits.h"
#include "probfd/types.h"

#include "downward/utils/hash.h"

#include <ostream>

namespace probfd {
namespace heuristics {
namespace pdbs {

/**
 * @brief Represents the rank of an abstract state.
 *
 * The rank of an abstract state is its unique index in \{ 0, \dots, N-1 \},
 * where N is the number of abstract states in the abstract state space.
 *
 * @see StateRankingFunction
 */
struct StateRank {
    long long int id;

    explicit StateRank(long long int id)
        : id(id)
    {
    }

    StateRank operator-(StateRank x) const { return StateRank(id - x.id); }

    friend auto operator<=>(StateRank a, StateRank b) = default;
};

extern std::ostream& operator<<(std::ostream& out, StateRank s);

} // namespace pdbs
} // namespace heuristics

template <>
struct is_cheap_to_copy<heuristics::pdbs::StateRank> : std::true_type {};

} // namespace probfd

namespace std {
template <>
struct hash<probfd::heuristics::pdbs::StateRank> {
    size_t operator()(probfd::heuristics::pdbs::StateRank state) const
    {
        return hash<long long int>()(state.id);
    }
};
} // namespace std

namespace utils {
void feed(HashState& h, probfd::heuristics::pdbs::StateRank s);
} // namespace utils

#endif // __ABSTRACT_STATE_H__