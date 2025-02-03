#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_INTERVAL_ITERATION_H
#error "This file should only be included from interval_iteration.h"
#endif

#include "probfd/quotients/quotient_max_heuristic.h"

#include "probfd/utils/not_implemented.h"

#include "probfd/progress_report.h"

#include "downward/utils/collections.h"

#include <iterator>

namespace probfd::algorithms::interval_iteration {

template <typename State, typename Action>
IntervalIteration<State, Action>::IntervalIteration(
    value_t epsilon,
    bool extract_probability_one_states,
    bool expand_goals)
    : extract_probability_one_states_(extract_probability_one_states)
    , qr_analysis_(expand_goals)
    , ec_decomposer_(expand_goals)
    , vi_(epsilon, expand_goals)
{
}

template <typename State, typename Action>
Interval IntervalIteration<State, Action>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    ParamType<State> state,
    ProgressReport,
    double max_time)
{
    utils::CountdownTimer timer(max_time);
    std::unique_ptr sys = create_quotient(mdp, heuristic, state, timer);
    std::vector<StateID> dead, one;
    storage::PerStateStorage<Interval> value_store;
    return mysolve(mdp, heuristic, state, value_store, dead, one, *sys, timer);
}

template <typename State, typename Action>
auto IntervalIteration<State, Action>::compute_policy(
    MDPType&,
    EvaluatorType&,
    ParamType<State>,
    ProgressReport,
    double) -> std::unique_ptr<PolicyType>
{
    not_implemented();
}

template <typename State, typename Action>
void IntervalIteration<State, Action>::print_statistics(std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action>
template <typename ValueStoreT, typename SetLike, typename SetLike2>
Interval IntervalIteration<State, Action>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    ParamType<State> state,
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
        const auto value = value_store[repr_id];
        const bool dead = utils::contains(dead_ends, repr_id);
        const bool one = utils::contains(one_states, repr_id);

        sys->for_each_member_state(
            repr_id,
            [&, value, dead, one](StateID member_id) {
                value_store[member_id] = value;
                if (dead) dead_ends.push_back(member_id);
                if (one) one_states.push_back(member_id);
            });
    }

    return x;
}

template <typename State, typename Action>
auto IntervalIteration<State, Action>::create_quotient(
    MDPType& mdp,
    EvaluatorType& heuristic,
    ParamType<State> state,
    utils::CountdownTimer& timer) -> std::unique_ptr<QSystem>
{
    auto sys = ec_decomposer_.build_quotient_system(
        mdp,
        &heuristic,
        state,
        timer.get_remaining_time());

    statistics_.ecd_statistics = ec_decomposer_.get_statistics();

    return sys;
}

template <typename State, typename Action>
template <typename ValueStoreT, typename SetLike, typename SetLike2>
Interval IntervalIteration<State, Action>::mysolve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    ParamType<State> state,
    ValueStoreT& value_store,
    SetLike& dead_ends,
    SetLike2& one_states,
    QSystem& sys,
    utils::CountdownTimer& timer)
{
    QState qstate = sys.translate_state(state);

    if (extract_probability_one_states_) {
        qr_analysis_.run_analysis(
            sys,
            nullptr,
            qstate,
            std::back_inserter(dead_ends),
            iterators::discarding_output_iterator(),
            std::back_inserter(one_states),
            timer.get_remaining_time());
        assert(mdp.get_termination_info(mdp.get_state(one_states.front()))
                   .is_goal_state());
    } else {
        qr_analysis_.run_analysis(
            sys,
            nullptr,
            qstate,
            std::back_inserter(dead_ends),
            iterators::discarding_output_iterator(),
            iterators::discarding_output_iterator(),
            timer.get_remaining_time());
    }

    assert(::utils::is_unique(dead_ends) && ::utils::is_unique(one_states));

    sys.build_quotient(dead_ends);
    sys.build_quotient(one_states);

    const auto new_init_id = sys.translate_state_id(mdp.get_state_id(state));

    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);

    const Interval result = vi_.solve(
        sys,
        qheuristic,
        new_init_id,
        value_store,
        timer.get_remaining_time());
    statistics_.tvi_statistics = vi_.get_statistics();
    return result;
}

} // namespace probfd::algorithms::interval_iteration
