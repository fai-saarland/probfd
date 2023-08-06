#ifndef GUARD_INCLUDE_PROBFD_ENGINES_INTERVAL_ITERATION_H
#error "This file should only be included from interval_iteration.h"
#endif

#include <iterator>

namespace probfd {
namespace engines {
namespace interval_iteration {

template <typename State, typename Action>
IntervalIteration<State, Action>::IntervalIteration(
    bool extract_probability_one_states,
    bool expand_goals)
    : extract_probability_one_states_(extract_probability_one_states)
    , expand_goals_(expand_goals)
    , qr_analysis(expand_goals)
    , vi(expand_goals)
{
}

template <typename State, typename Action>
Interval IntervalIteration<State, Action>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    double max_time)
{
    utils::CountdownTimer timer(max_time);
    std::unique_ptr sys = create_quotient(mdp, heuristic, state, timer);
    std::vector<StateID> dead, one;
    return mysolve(mdp, heuristic, state, value_store_, dead, one, *sys, timer);
}

template <typename State, typename Action>
void IntervalIteration<State, Action>::print_statistics(std::ostream& out) const
{
    tvi_statistics_.print(out);
    ecd_statistics_.print(out);
}

template <typename State, typename Action>
template <typename ValueStoreT, typename SetLike, typename SetLike2>
Interval IntervalIteration<State, Action>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    ValueStoreT& value_store,
    SetLike& dead_ends,
    SetLike2& one_states,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    auto sys = create_quotient(mdp, heuristic, state, timer);

    const Interval x = this->mysolve(
        mdp,
        heuristic,
        state,
        value_store,
        dead_ends,
        one_states,
        *sys,
        timer);

    for (StateID repr_id : *sys) {
        auto [sit, send] = sys->quotient_range(repr_id);
        const StateID repr = *sit;

        const auto value = value_store[repr];
        const bool dead = utils::contains(dead_ends, repr);
        const bool one = utils::contains(one_states, repr);
        for (++sit; sit != send; ++sit) {
            value_store[*sit] = value;
            if (dead) dead_ends.push_back(*sit);
            if (one) one_states.push_back(*sit);
        }
    }

    return x;
}

template <typename State, typename Action>
auto IntervalIteration<State, Action>::create_quotient(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    utils::CountdownTimer& timer) -> std::unique_ptr<QuotientSystem>
{
    Decomposer ec_decomposer(expand_goals_, &heuristic);

    auto sys = ec_decomposer.build_quotient_system(
        mdp,
        state,
        timer.get_remaining_time());

    ecd_statistics_ = ec_decomposer.get_statistics();

    return sys;
}

template <typename State, typename Action>
template <typename ValueStoreT, typename SetLike, typename SetLike2>
Interval IntervalIteration<State, Action>::mysolve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    ValueStoreT& value_store,
    SetLike& dead_ends,
    SetLike2& one_states,
    QuotientSystem& sys,
    utils::CountdownTimer timer)
{
    if (extract_probability_one_states_) {
        qr_analysis.run_analysis(
            sys,
            state,
            std::back_inserter(dead_ends),
            iterators::discarding_output_iterator(),
            std::back_inserter(one_states),
            timer.get_remaining_time());
        assert(mdp.get_termination_info(mdp.get_state(one_states.front()))
                   .is_goal_state());
    } else {
        qr_analysis.run_analysis(
            sys,
            state,
            std::back_inserter(dead_ends),
            iterators::discarding_output_iterator(),
            iterators::discarding_output_iterator(),
            timer.get_remaining_time());
    }

    assert(::utils::is_unique(dead_ends) && ::utils::is_unique(one_states));

    sys.build_quotient(dead_ends);
    sys.build_quotient(one_states);

    const auto new_init_id = sys.translate_state_id(mdp.get_state_id(state));

    const Interval result = vi.solve(
        sys,
        heuristic,
        new_init_id,
        value_store,
        timer.get_remaining_time());
    tvi_statistics_ = vi.get_statistics();
    return result;
}

} // namespace interval_iteration
} // namespace engines
} // namespace probfd