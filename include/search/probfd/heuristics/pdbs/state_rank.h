#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_H

#include "utils/hash.h"

#include <ostream>

namespace probfd {
namespace heuristics {
namespace pdbs {

/**
 * @brief Represents the rank of a state.
 *
 * The rank of a state is its unique index in \{ 0, \dots, N-1 \}, where N is
 * the number of states in the state space.
 *
 * @see StateRankingFunction
 */
struct StateRank {
    long long int id;

    explicit StateRank(long long int id)
        : id(id)
    {
    }

    StateRank operator+(const StateRank& x) const
    {
        return StateRank(x.id + id);
    }

    StateRank operator-(const StateRank& x) const
    {
        return StateRank(id - x.id);
    }

    StateRank& operator+=(const StateRank& x)
    {
        id += x.id;
        return *this;
    }

    StateRank& operator-=(const StateRank& x)
    {
        id -= x.id;
        return *this;
    }

    friend auto operator<=>(StateRank a, StateRank b) = default;
};

extern std::ostream& operator<<(std::ostream& out, const StateRank& s);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

namespace std {
template <>
struct hash<probfd::heuristics::pdbs::StateRank> {
    size_t operator()(const probfd::heuristics::pdbs::StateRank& state) const
    {
        return hash<long long int>()(state.id);
    }
};
} // namespace std

namespace utils {
void feed(HashState& h, const probfd::heuristics::pdbs::StateRank& s);
} // namespace utils

#endif // __ABSTRACT_STATE_H__