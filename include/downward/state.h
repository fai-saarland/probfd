#ifndef DOWNWARD_STATE_H
#define DOWNWARD_STATE_H

#include "downward/axiom_evaluator.h"
#include "downward/proxy_collection.h"
#include "downward/state_id.h"
#include "downward/variable_space.h"

#include "downward/algorithms/int_packer.h"

#include "downward/utils/hash.h"
#include "downward/utils/system.h"

#include "probfd/views/convert.h"
#include "probfd/views/enumerate.h"

#include <cassert>
#include <compare>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <vector>

namespace downward {
class State;
class StateRegistry;
class VariableProxy;
} // namespace downward

namespace downward {

using PackedStateBin = int_packer::IntPacker::Bin;

template <typename T>
concept OperatorLike =
    requires(const T& op, const State& state, AxiomEvaluator& axiom_evaluator) {
        {
            op.get_unregistered_successor(state, axiom_evaluator)
        } -> std::same_as<State>;
    };

extern void
apply_axioms(const PlanningTask& task, std::vector<int>& values);

template <std::ranges::input_range FactRange, typename StateLike>
bool all_facts_true(const FactRange& facts, const StateLike& state)
    requires std::
        convertible_to<std::ranges::range_reference_t<FactRange>, FactPair>
{
    for (const auto [var, value] : facts) {
        if (state[var] != value) return false;
    }
    return true;
}

template <typename ConditionalEffects, typename StateLike>
void apply_conditional_effects(
    const ConditionalEffects& effects,
    const StateLike& condition_state,
    std::vector<int>& values)
{
    for (const auto effect : effects) {
        if (all_facts_true(effect.get_conditions(), condition_state)) {
            const auto [var, value] = effect.get_fact();
            values[var] = value;
        }
    }
}

class State : public ProxyCollection<State> {
    /*
      TODO: We want to try out two things:
        1. having StateID and num_variables next to each other, so that they
           can fit into one 8-byte-aligned block.
        2. removing num_variables altogether, getting it on demand.
           It is not used in (very) performance-critical code.
      Perhaps 2) has no positive influence after 1) because we would just have
      a 4-byte gap at the end of the object. But it would probably be nice to
      have fewer attributes regardless.
    */
    const StateRegistry* registry;
    StateID id;
    const PackedStateBin* buffer;
    /*
      values is mutable because we think of it as a redundant representation
      of the state's contents, a kind of cache. One could argue for doing this
      differently (for example because some methods require unpacked data, so
      in some sense its presence changes the "logical state" from the program
      perspective. It is a bit weird to have a const function like unpack that
      is only called for its side effect on the object. But we decided to use
      const here to mean "const from the perspective of the state space
      semantics of the state".
    */
    mutable std::shared_ptr<std::vector<int>> values;
    const int_packer::IntPacker* state_packer;
    int num_variables;

public:
    // Construct a registered state with only packed data.
    State(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer);

    // Construct a registered state with packed and unpacked data.
    State(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer,
        std::vector<int>&& values);

    // Construct a state with only unpacked data.
    explicit State(std::vector<int>&& values);

    /* Generate unpacked data if it is not available yet. Calling the function
       on a state that already has unpacked data has no effect. */
    void unpack() const;

    std::size_t size() const;

    int operator[](std::size_t var_id) const;
    int operator[](VariableProxy var) const;

    /* Return a pointer to the registry in which this state is registered.
       If the state is not registered, return nullptr. */
    const StateRegistry* get_registry() const;

    /* Return the ID of the state within its registry. If the state is not
       registered, return StateID::no_state. */
    StateID get_id() const;

    /* Access the unpacked values. Accessing the unpacked values in a state
       that doesn't have them is an error. Use unpack() to ensure the data
       exists. */
    const std::vector<int>& get_unpacked_values() const;

    /* Access the packed values. Accessing packed values on states that do
       not have them (unregistered states) is an error. */
    const PackedStateBin* get_buffer() const;

    template <OperatorLike O>
    State
    get_unregistered_successor(AxiomEvaluator& axiom_evaluator, const O& op)
        const
    {
        return op.get_unregistered_successor(*this, axiom_evaluator);
    }

    template <typename ConditionalEffects>
    State get_unregistered_successor(
        AxiomEvaluator& axiom_evaluator,
        const ConditionalEffects& effects) const
    {
        assert(values);
        std::vector<int> new_values = get_unpacked_values();
        apply_conditional_effects(effects, *this, new_values);
        axiom_evaluator.evaluate(new_values);
        return State(std::move(new_values));
    }

    friend bool operator==(const State& left, const State& right);
};

class StateEnumerateFn
    : public std::ranges::range_adaptor_closure<StateEnumerateFn> {
public:
    [[nodiscard]]
    auto operator()(const State& state) const
        noexcept(noexcept(probfd::views::enumerate_view{state}))
    {
        return probfd::views::enumerate_view{state};
    }

    auto operator()(State&& state) const
        noexcept(noexcept(probfd::views::enumerate_view{std::move(state)}))
    {
        return probfd::views::enumerate_view{std::move(state)};
    }
};

inline constexpr auto as_fact_pair_set =
    StateEnumerateFn{} |
    // std::views::transform([](const auto& p) { return FactPair(p);});
    probfd::views::convert<FactPair>;

namespace utils {
inline void feed(HashState& hash_state, const State& state)
{
    /*
      Hashing a state without unpacked data will result in an error.
      We don't want to unpack states implicitly, so this rules out the option
      of unpacking the states here on demand. Mixing hashes from packed and
      unpacked states would lead to logically equal states with different
      hashes. Hashing packed (and therefore registered) states also seems like
      a performance error because it's much cheaper to hash the state IDs
      instead.
    */
    feed(hash_state, state.get_unpacked_values());
}
} // namespace utils

inline bool operator==(const State& left, const State& right)
{
    if (left.registry != right.registry) {
        std::cerr << "Comparing registered states with unregistered states "
                  << "or registered states from different registries is "
                  << "treated as an error because it is likely not "
                  << "intentional." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    if (left.registry) {
        // Both states are registered and from the same registry.
        return left.id == right.id;
    } else {
        // Both states are unregistered.
        assert(left.values);
        assert(right.values);
        return *left.values == *right.values;
    }
}

inline void State::unpack() const
{
    if (!values) {
        int num_variables = size();
        /*
          A micro-benchmark in issue348 showed that constructing the vector
          in the required size and then assigning values was faster than the
          more obvious reserve/push_back. Although, the benchmark did not
          profile this specific code.

          We might consider a bulk-unpack method in state_packer that could be
          more efficient. (One can imagine state packer to have extra data
          structures that exploit sequentially unpacking each entry, by doing
          things bin by bin.)
        */
        values = std::make_shared<std::vector<int>>(num_variables);
        for (int var = 0; var < num_variables; ++var) {
            (*values)[var] = state_packer->get(buffer, var);
        }
    }
}

inline std::size_t State::size() const
{
    return num_variables;
}

inline int State::operator[](std::size_t var_id) const
{
    assert(var_id < size());
    if (values) {
        return (*values)[var_id];
    } else {
        assert(buffer);
        assert(state_packer);
        return state_packer->get(buffer, var_id);
    }
}

inline int State::operator[](VariableProxy var) const
{
    return (*this)[var.get_id()];
}

inline const StateRegistry* State::get_registry() const
{
    return registry;
}

inline StateID State::get_id() const
{
    return id;
}

inline const PackedStateBin* State::get_buffer() const
{
    /*
      TODO: we should profile what happens if we #ifndef NDEBUG this test here
      and in other places (e.g. the next method). The 'if' itself is probably
      not costly, but the 'cerr <<' stuff might prevent inlining.
    */
    if (!buffer) {
        std::cerr << "Accessing the packed values of an unregistered state is "
                  << "treated as an error." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    return buffer;
}

inline const std::vector<int>& State::get_unpacked_values() const
{
    if (!values) {
        std::cerr << "Accessing the unpacked values of a state without "
                  << "unpacking them first is treated as an error. Please "
                  << "use State::unpack first." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    return *values;
}

} // namespace downward

#endif
