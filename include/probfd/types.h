#ifndef PROBFD_TYPES_H
#define PROBFD_TYPES_H

#include "probfd/concepts.h"

#include "downward/operator_id.h"
#include "downward/state_id.h"

#include <cstdint>
#include <functional>
#include <limits>
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
    static constexpr size_type UNDEFINED =
        std::numeric_limits<size_type>::max();

    StateID(size_type id = StateID::UNDEFINED)
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

template <>
constexpr bool enable_pass_by_value<StateID> = true;

template <>
constexpr bool enable_pass_by_value<OperatorID> = true;

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

#endif // PROBFD_TYPES_H