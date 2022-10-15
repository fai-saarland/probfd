#ifndef STATE_ID_H
#define STATE_ID_H

#include <iostream>

// For documentation on classes relevant to storing and working with registered
// states see the file state_registry.h.

class StateID {
    friend class StateRegistry;
    friend std::ostream &operator<<(std::ostream &os, StateID id);
    template<typename>
    friend class PerStateInformation;

    int value;

public:
    StateID() : value(StateID::no_state.value) {}
    explicit StateID(int value_)
        : value(value_) {
    }
    ~StateID() {
    }

    static const StateID no_state;

    bool operator==(const StateID &other) const {
        return value == other.value;
    }

    bool operator!=(const StateID &other) const {
        return !(*this == other);
    }

    size_t hash() const {
        return value;
    }
};

std::ostream &operator<<(std::ostream &os, StateID id);

namespace std {
template<>
struct hash<StateID> {
    size_t operator()(const StateID& state_id) const { return state_id.hash(); }
};

}

#endif
