#ifndef PROBFD_STATE_ID_H
#define PROBFD_STATE_ID_H

#include "probfd/concepts.h"

#include "downward/state_id.h"

#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>

namespace probfd {

/**
 * @brief A consecutive ID type for states.
 */
struct StateID {
    using size_type = unsigned long long;
    static constexpr size_type UNDEFINED =
        std::numeric_limits<size_type>::max();

    StateID(size_type id = StateID::UNDEFINED)
        : id(id)
    {
    }

    StateID(downward::StateID id)
        : id(id.get_value())
    {
    }

    operator downward::StateID() const { return downward::StateID(id); }

    operator size_type() const { return id; }

    size_type id;
};

template <>
inline constexpr bool enable_pass_by_value<StateID> = true;

} // namespace probfd

template <>
struct std::hash<probfd::StateID> {
    size_t operator()(const probfd::StateID& sid) const noexcept
    {
        return hash<probfd::StateID::size_type>()(sid);
    }
};

#endif // PROBFD_STATE_ID_H