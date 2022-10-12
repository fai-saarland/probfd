#ifndef MDPS_TYPES_H
#define MDPS_TYPES_H

#include <cstdint>
#include <functional>
#include <type_traits>

namespace probfd {

struct StateID {
    using size_type = uint32_t;
    static constexpr size_type undefined = -1;

    StateID(size_type id = StateID::undefined)
        : id(id)
    {
    }

    operator size_type() const { return id; }

    size_type id;
};

struct ActionID {
    using size_type = uint32_t;
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