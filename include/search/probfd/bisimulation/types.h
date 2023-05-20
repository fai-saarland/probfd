#ifndef PROBFD_BISIMULATION_TYPES_H
#define PROBFD_BISIMULATION_TYPES_H

#include "probfd/type_traits.h"
#include "probfd/types.h"

#include <compare>

namespace probfd {

/// This namespace contains the implementation of deterministic bisimulation
/// quotients for SSPs, based on merge-and-shrink.
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
};

/// Represents an action in the probabilistic bisimulation quotient.
struct QuotientAction {
    unsigned idx; ///< Numbering of this action

    explicit QuotientAction(unsigned idx)
        : idx(idx)
    {
    }

    friend auto
    operator<=>(const QuotientAction&, const QuotientAction&) = default;
};

} // namespace bisimulation

template <>
struct is_cheap_to_copy<bisimulation::QuotientState> : std::true_type {};

template <>
struct is_cheap_to_copy<bisimulation::QuotientAction> : std::true_type {};

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