#pragma once

#include <cstdint>
#include <functional>
#include <type_traits>

namespace probabilistic {

struct StateID {
public:
    using size_type = uint32_t;
    constexpr static const size_type undefined = -1;

    StateID()
        : id(undefined)
    {
    }

    StateID(const size_type& id)
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
    constexpr static const size_type undefined = -1;

    ActionID()
        : id(undefined)
    {
    }

    ActionID(const size_type& id)
        : id(id)
    {
    }

    ActionID& operator=(const size_type& sid)
    {
        id = sid;
        return *this;
    }

    operator size_type() const { return id; }

    size_type id;
};

template<typename T, typename B = std::true_type>
struct is_default_implementation : public std::false_type {
};

template<typename T>
struct is_default_implementation<T, typename T::is_default_implementation>
    : public std::true_type {
};

} // namespace probabilistic

namespace std {

template<>
struct hash<probabilistic::StateID> {
    size_t operator()(const probabilistic::StateID& sid) const
    {
        return hash<probabilistic::StateID::size_type>()(sid);
    }
};

template<>
struct hash<probabilistic::ActionID> {
    size_t operator()(const probabilistic::ActionID& aid) const
    {
        return hash<probabilistic::ActionID::size_type>()(aid);
    }
};

} // namespace std

