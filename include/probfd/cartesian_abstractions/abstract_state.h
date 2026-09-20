#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_ABSTRACT_STATE_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_ABSTRACT_STATE_H

#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_set.h"

#include <concepts>
#include <format>
#include <iosfwd>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

// Forward Declarations
namespace downward {
struct FactPair;
class State;
} // namespace downward

namespace probfd {
class ProbabilisticOperatorProxy;
class ProbabilisticEffectsProxy;
} // namespace probfd

namespace probfd::cartesian_abstractions {

/*
  Store the Cartesian set and the ID of the node in the refinement hierarchy
  for an abstract state.
*/
class AbstractState {
    int state_id_;

    // This state's node in the refinement hierarchy.
    NodeID node_id_;

    CartesianSet cartesian_set_;

    friend std::formatter<AbstractState>;

public:
    AbstractState(int state_id, NodeID node_id, CartesianSet cartesian_set);

    AbstractState(const AbstractState&) = delete;
    AbstractState& operator=(const AbstractState&) = delete;

    AbstractState(AbstractState&&) noexcept = default;
    AbstractState& operator=(AbstractState&&) noexcept = default;

    [[nodiscard]]
    bool domain_subsets_intersect(const AbstractState& other, int var) const;

    // Return the size of var's abstract domain for this state.
    [[nodiscard]]
    int count(int var) const;

    [[nodiscard]]
    bool contains(int var, int value) const;

    // IDs are consecutive, so they can be used to index states in vectors.
    [[nodiscard]]
    int get_id() const;

    [[nodiscard]]
    NodeID get_node_id() const;

    [[nodiscard]]
    const CartesianSet& get_cartesian_set() const;

    friend std::ostream&
    operator<<(std::ostream& os, const AbstractState& state);

    // Create the initial, unrefined abstract state.
    template <std::ranges::input_range R>
        requires std::same_as<std::ranges::range_value_t<R>, int>
    static AbstractState get_trivial_abstract_state(const R& domain_sizes)
    {
        return AbstractState(0, 0, CartesianSet(domain_sizes));
    }
};

/*
  Separate the "wanted" values from the other values in the abstract domain
  and return the resulting two new Cartesian sets.
*/
[[nodiscard]]
std::pair<CartesianSet, CartesianSet> split_cart_state(
    const CartesianSet& cartesian_set,
    int var,
    const std::vector<int>& wanted);

// Return the Cartesian set in which applying "effect" of the operator "op"
// can lead to this state.
[[nodiscard]]
CartesianSet regress(
    CartesianSet cartesian_set,
    const ProbabilisticOperatorProxy& op,
    const ProbabilisticEffectsProxy& effects);

} // namespace probfd::cartesian_abstractions

template <>
struct std::formatter<probfd::cartesian_abstractions::AbstractState> {
    template <class ParseContext>
    static constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format(
        const probfd::cartesian_abstractions::AbstractState& state,
        FmtContext& ctx) const
    {
        return std::format_to(
            ctx.out(),
            "#{} {}",
            state.get_id(),
            state.cartesian_set_);
    }
};

static_assert(
    std::formattable<probfd::cartesian_abstractions::AbstractState, char>);

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_ABSTRACT_STATE_H
