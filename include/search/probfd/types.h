#ifndef PROBFD_TYPES_H
#define PROBFD_TYPES_H

#include <cstdint>
#include <functional>
#include <type_traits>

namespace probfd {

/// Integer ID used to represent a state cheaply.

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

    operator size_type() const { return id; }

    size_type id;
};

/**
 * @brief An ActionID represents an action within an ActionIDMap. It is trivial
 * to copy and intended for long term storage.
 *
 * @see engine_interfaces::ActionIDMap
 */
struct ActionID {
    using size_type = std::make_unsigned_t<std::ptrdiff_t>;
    static constexpr size_type undefined = -1;

    ActionID(size_type id = ActionID::undefined)
        : id(id)
    {
    }

    operator size_type() const { return id; }

    size_type id;
};

} // namespace probfd

namespace std {

template <>
struct hash<probfd::StateID> {
    size_t operator()(const probfd::StateID& sid) const
    {
        return hash<probfd::StateID::size_type>()(sid);
    }
};

template <>
struct hash<probfd::ActionID> {
    size_t operator()(const probfd::ActionID& aid) const
    {
        return hash<probfd::ActionID::size_type>()(aid);
    }
};

} // namespace std

#endif // __TYPES_H__