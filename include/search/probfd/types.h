#ifndef PROBFD_TYPES_H
#define PROBFD_TYPES_H

#include "downward/operator_id.h"
#include "downward/state_id.h"

#include <cstdint>
#include <functional>
#include <type_traits>

namespace probfd {

/**
 * @brief A StateID represents a state within a StateIDMap. Just like Fast
 * Downward's StateID type, it is trivial to copy and intended for long term
 * storage.
 *
 * @see StateIDMap
 */

struct StateID {
    using size_type = unsigned long long;
    static constexpr size_type undefined = -1;

    StateID(size_type id = StateID::undefined)
        : id(id)
    {
    }

    StateID(::StateID id)
        : id(id.get_value())
    {
    }

    operator ::StateID() const { return ::StateID(id); }

    operator size_type() const { return id; }

    size_type id;
};

} // namespace probfd

inline auto operator<=>(OperatorID left, OperatorID right)
{
    return left.get_index() <=> right.get_index();
}

namespace std {

template <>
struct hash<probfd::StateID> {
    size_t operator()(const probfd::StateID& sid) const
    {
        return hash<probfd::StateID::size_type>()(sid);
    }
};

} // namespace std

#endif // __TYPES_H__