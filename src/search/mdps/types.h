#ifndef MDPS_TYPES_H
#define MDPS_TYPES_H

#include <cstdint>
#include <functional>
#include <type_traits>

namespace probabilistic {

struct StateID {
public:
    using size_type = uint32_t;
    static constexpr size_type undefined = -1;

    StateID(const size_type& id = StateID::undefined)
        : id(id)
    {
    }

    StateID& operator=(const size_type& sid)
    {
        id = sid;
        return *this;
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

    ActionID& operator=(size_type sid)
    {
        id = sid;
        return *this;
    }

    operator size_type() const { return id; }

    size_type id;
};

} // namespace probabilistic

namespace std {

template <>
struct hash<probabilistic::StateID> {
    size_t operator()(const probabilistic::StateID& sid) const
    {
        return hash<probabilistic::StateID::size_type>()(sid);
    }
};

template <>
struct hash<probabilistic::ActionID> {
    size_t operator()(const probabilistic::ActionID& aid) const
    {
        return hash<probabilistic::ActionID::size_type>()(aid);
    }
};

} // namespace std

#endif // __TYPES_H__