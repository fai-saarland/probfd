#ifndef MDPS_BISIMULATION_TYPES_H
#define MDPS_BISIMULATION_TYPES_H

#include "probfd/types.h"

namespace probfd {

/// Namespace dedicated to probabilistic bisimulation.
namespace bisimulation {

/// Represents a state in the probabilistic bisimulation quotient.
class QuotientState : public StateID {
public:
    explicit QuotientState(StateID id = StateID::undefined)
        : StateID(id)
    {
    }

    QuotientState& operator=(StateID id)
    {
        StateID::operator=(id);
        return *this;
    }

    bool operator==(const StateID& s) const { return id == s.id; }
    bool operator!=(const StateID& s) const { return id != s.id; }
    bool operator<(const StateID& s) const { return id < s.id; }
};

/// Represents an action in the probabilistic bisimulation quotient.
struct QuotientAction {
    explicit QuotientAction(unsigned idx)
        : idx(idx)
    {
    }

    bool operator==(const QuotientAction& o) const { return o.idx == idx; }

    unsigned idx; ///< Numbering of this action
};

} // namespace bisimulation
} // namespace probfd

namespace std {
template <>
struct hash<probfd::bisimulation::QuotientState> {
    size_t operator()(const probfd::bisimulation::QuotientState& s) const
    {
        return s.id;
    }
};
} // namespace std

#endif