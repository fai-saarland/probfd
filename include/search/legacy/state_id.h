#ifndef LEGACY_STATE_ID_H
#define LEGACY_STATE_ID_H

#include <iostream>

// For documentation on classes relevant to storing and working with registered
// states see the file state_registry.h.

namespace legacy {
class StateRegistry;

class StateID {
    friend class legacy::StateRegistry;
    friend std::ostream& operator<<(std::ostream& os, StateID id);
    template <typename>
    friend class PerStateInformation;

    int value;

public:
    StateID()
        : value(StateID::no_state.value)
    {
    }
    explicit StateID(int value_)
        : value(value_)
    {
    }
    ~StateID() {}

    static const StateID no_state;

    bool operator==(const StateID& other) const { return value == other.value; }

    bool operator!=(const StateID& other) const { return !(*this == other); }

    size_t hash() const { return value; }
};

std::ostream& operator<<(std::ostream& os, StateID id);
} // namespace legacy

namespace std {
template <>
struct hash<legacy::StateID> {
    size_t operator()(const legacy::StateID& state_id) const
    {
        return state_id.hash();
    }
};

} // namespace std

#endif
