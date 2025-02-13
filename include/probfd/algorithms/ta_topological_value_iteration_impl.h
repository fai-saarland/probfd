#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H
#error "This file should only be included from ta_topological_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/utils/guards.h"
#include "probfd/utils/iterators.h"
#include "probfd/utils/not_implemented.h"

#include "probfd/cost_function.h"
#include "probfd/heuristic.h"
#include "probfd/progress_report.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>

namespace probfd::algorithms::ta_topological_vi {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Expanded state(s): " << expanded_states << std::endl;
    out << "  Terminal state(s): " << terminal_states << std::endl;
    out << "  Goal state(s): " << goal_states << std::endl;
    out << "  Pruned state(s): " << pruned << std::endl;
    out << "  Maximal SCCs: " << sccs << " (" << singleton_sccs
        << " are singleton)" << std::endl;
    out << "  Bellman backups: " << bellman_backups << std::endl;

    out << "  Time spent initializing state data: " << initialize_state_timer
        << std::endl;
    out << "  Time spent expanding successors: " << successor_handling_timer
        << std::endl;
    out << "  Time spent handling SCCs: " << scc_handling_timer << std::endl;
    out << "  Time spent backtracking: " << backtracking_timer << std::endl;
    out << "  Time spent running VI on SCCs: " << vi_timer << std::endl;
    out << "  Time spent decomposing ECs: " << decomposition_timer << std::endl;
    out << "  Time spent in solvability analysis: " << solvability_timer
        << std::endl;
}

template <typename State, typename Action, bool UseInterval>
auto TATopologicalValueIteration<State, Action, UseInterval>::StateInfo::
    get_status() const
{
    return explored ? (stack_id < UNDEF ? ONSTACK : CLOSED) : NEW;
}

template <typename State, typename Action, bool UseInterval>
auto TATopologicalValueIteration<State, Action, UseInterval>::StateInfo::
    get_ecd_status() const
{
    return explored ? (ecd_stack_id < UNDEF_ECD ? ONSTACK : CLOSED) : NEW;
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    ExplorationInfo(
        StateID state_id,
        StackInfo& stack_info,
        unsigned int stackidx)
    : state_id(state_id)
    , stack_info(stack_info)
    , stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_transition(MDPType& mdp)
{
    aops.pop_back();
    successor_dist.clear();

    assert(q_value.scc_successors.empty());

    return !aops.empty() &&
           forward_non_loop_transition(mdp, mdp.get_state(state_id));
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    forward_non_loop_transition(MDPType& mdp, const State& state)
{
    do {
        mdp.generate_action_transitions(state, aops.back(), successor_dist);

        if (!successor_dist.non_source_successor_dist.empty()) {
            successor = successor_dist.non_source_successor_dist.begin();

            const auto cost = mdp.get_action_cost(aops.back());
            if (cost != 0.0_vt) has_all_zero = false;
            q_value.conv_part = AlgorithmValueType(cost);
            q_value.normalization = 1 / successor_dist.non_source_probability;
            return true;
        }

        aops.pop_back();
        successor_dist.clear();
    } while (!aops.empty());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_successor()
{
    if (++successor != successor_dist.non_source_successor_dist.end()) {
        return true;
    }

    const bool exits_only_solvable =
        q_value.conv_part != AlgorithmValueType(INFINITE_VALUE);

    const size_t num_scc_succs = q_value.scc_successors.size();

    if (num_scc_succs == 0) {
        // Universally exiting -> Not part of scc
        // Update converged portion of q value and ignore this
        // transition
        set_min(
            stack_info.conv_part,
            q_value.normalization * q_value.conv_part);

        if (exits_only_solvable) {
            ++stack_info.active_exit_transitions;
            ++stack_info.active_transitions;
        }
    } else {
        const bool leaves_scc =
            num_scc_succs != successor_dist.non_source_successor_dist.size();

        if (leaves_scc || q_value.conv_part != AlgorithmValueType(0_vt)) {
            // Only some exiting or cost is non-zero ->
            // Not part of an end component
            // Add the transition to the set of non-EC transitions
            stack_info.add_non_ec_transition(std::move(q_value));
        } else {
            // Otherwise add to EC transitions
            stack_info.ec_transitions.emplace_back(std::move(q_value));
        }

        if (exits_only_solvable) {
            if (leaves_scc) {
                ++stack_info.active_exit_transitions;
            }
            ++stack_info.active_transitions;
        }
        stack_info.transition_flags.emplace_back(
            exits_only_solvable && leaves_scc,
            exits_only_solvable);
    }

    assert(q_value.scc_successors.empty());

    return false;
}

template <typename State, typename Action, bool UseInterval>
ItemProbabilityPair<StateID>
TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    get_current_successor()
{
    return *successor;
}

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
auto TATopologicalValueIteration<State, Action, UseInterval>::QValueInfo::
    compute_q_value(ValueStore& value_store) const -> AlgorithmValueType
{
    AlgorithmValueType res = conv_part;

    for (auto& [state_id, prob] : scc_successors) {
        res += prob * value_store[state_id];
    }

    return normalization * res;
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::StackInfo::StackInfo(
    StateID state_id,
    AlgorithmValueType& value_ref)
    : state_id(state_id)
    , value(&value_ref)
{
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::StackInfo::
    add_non_ec_transition(QValueInfo&& info)
{
    auto [it, inserted] = non_ec_transitions.insert(std::move(info));
    if (!inserted) {
        // Not moved if insertion didn't take place
        set_min(it->conv_part, info.conv_part);
        info.scc_successors.clear();
    }
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::ECDExplorationInfo::
    ECDExplorationInfo(StackInfo& stack_info, unsigned stackidx)
    : stack_info(stack_info)
    , stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    ECDExplorationInfo::next_transition()
{
    assert(!action->scc_successors.empty());
    assert(action < end);

    if (!leaves_scc) {
        if (++action != end) {
            successor = action->scc_successors.begin();
            assert(!action->scc_successors.empty());

            remains_scc = false;

            return true;
        }
    } else {
        if (remains_scc) {
            recurse = true;
        }

        stack_info.add_non_ec_transition(std::move(*action));

        if (action != --end) {
            *action = std::move(*end);
            assert(!action->scc_successors.empty());
            successor = action->scc_successors.begin();

            leaves_scc = false;
            remains_scc = false;

            return true;
        }
    }

    auto& ect = stack_info.ec_transitions;
    ect.erase(end, ect.end());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    ECDExplorationInfo::next_successor()
{
    assert(!action->scc_successors.empty());
    return ++successor != action->scc_successors.end();
}

template <typename State, typename Action, bool UseInterval>
ItemProbabilityPair<StateID>
TATopologicalValueIteration<State, Action, UseInterval>::ECDExplorationInfo::
    get_current_successor()
{
    return *successor;
}

template <typename State, typename Action, bool UseInterval>
Interval TATopologicalValueIteration<State, Action, UseInterval>::solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
    ProgressReport,
    double max_time)
{
    storage::PerStateStorage<AlgorithmValueType> value_store;
    return this
        ->solve(mdp, heuristic, mdp.get_state_id(state), value_store, max_time);
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
Statistics
TATopologicalValueIteration<State, Action, UseInterval>::get_statistics() const
{
    return statistics_;
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::
    TATopologicalValueIteration(value_t epsilon)
    : IterativeMDPAlgorithm<State, Action>(epsilon)
{
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::
    TATopologicalValueIteration(value_t epsilon, std::size_t num_states_hint)
    : TATopologicalValueIteration(epsilon)
{
    exploration_stack_.reserve(num_states_hint);
    exploration_stack_ecd_.reserve(num_states_hint);
    stack_ecd_.reserve(num_states_hint);
    decomposition_queue_.reserve(num_states_hint);
    scc_.reserve(num_states_hint);
}

template <typename State, typename Action, bool UseInterval>
Interval TATopologicalValueIteration<State, Action, UseInterval>::solve(
    MDPType& mdp,
    const HeuristicType& heuristic,
    StateID init_state_id,
    auto& value_store,
    double max_time)
{
    statistics_ = Statistics();

    // scope_exit _([this] { statistics_.print(std::cout); });

    utils::CountdownTimer timer(max_time);

    push_state(
        init_state_id,
        state_information_[init_state_id],
        value_store[init_state_id]);

    for (;;) {
        ExplorationInfo* explore;

        do {
            explore = &exploration_stack_.back();
        } while (initialize_state(mdp, heuristic, *explore, value_store) &&
                 successor_loop(mdp, *explore, value_store, timer));

        // Iterative backtracking
        do {
            const unsigned stack_id = explore->stackidx;
            const unsigned lowlink = explore->lowlink;

            assert(stack_id >= lowlink);

            const bool backtrack_from_scc = stack_id == lowlink;

            // Check if an SCC was found.
            if (backtrack_from_scc) {
                scc_found(value_store, *explore, stack_id, timer);

                if (stack_id == 0) {
                    assert(stack_.empty());
                    assert(exploration_stack_.size() == 1);
                    exploration_stack_.pop_back();

                    if constexpr (UseInterval) {
                        return value_store[init_state_id];
                    } else {
                        return Interval(
                            value_store[init_state_id],
                            INFINITE_VALUE);
                    }
                }
            }

            assert(exploration_stack_.size() > 1);

            timer.throw_if_expired();

            TimerScope _(statistics_.backtracking_timer);

            const ExplorationInfo& successor = *explore--;

            const auto [succ_id, prob] = explore->get_current_successor();

            if (backtrack_from_scc) {
                const AlgorithmValueType value = value_store[succ_id];
                explore->q_value.conv_part += prob * value;
                explore->exit_interval.lower =
                    std::min(explore->exit_interval.lower, value);
                explore->exit_interval.upper =
                    std::max(explore->exit_interval.upper, value);
            } else {
                explore->lowlink = std::min(explore->lowlink, lowlink);
                explore->exit_interval.lower = std::min(
                    explore->exit_interval.lower,
                    successor.exit_interval.lower);
                explore->exit_interval.upper = std::max(
                    explore->exit_interval.upper,
                    successor.exit_interval.upper);
                explore->has_all_zero =
                    explore->has_all_zero && successor.has_all_zero;

                explore->q_value.scc_successors.emplace_back(succ_id, prob);
                successor.stack_info.parents.emplace_back(
                    explore->stackidx,
                    explore->stack_info.transition_flags.size());
            }

            exploration_stack_.pop_back();
        } while (
            (!explore->next_successor() && !explore->next_transition(mdp)) ||
            !successor_loop(mdp, *explore, value_store, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
auto TATopologicalValueIteration<State, Action, UseInterval>::compute_policy(
    MDPType&,
    HeuristicType&,
    ParamType<State>,
    ProgressReport,
    double) -> std::unique_ptr<PolicyType>
{
    not_implemented();
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::push_state(
    StateID state_id,
    StateInfo& state_info,
    AlgorithmValueType& value)
{
    const std::size_t stack_size = stack_.size();
    exploration_stack_.emplace_back(
        state_id,
        stack_.emplace_back(state_id, value),
        stack_size);
    state_info.explored = 1;
    state_info.stack_id = stack_size;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::successor_loop(
    MDPType& mdp,
    ExplorationInfo& explore,
    auto& value_store,
    utils::CountdownTimer& timer)
{
    TimerScope _(statistics_.successor_handling_timer);

    do {
        timer.throw_if_expired();

        const auto [succ_id, prob] = explore.get_current_successor();
        assert(succ_id != explore.state_id);

        StateInfo& succ_info = state_information_[succ_id];

        switch (succ_info.get_status()) {
        default: abort();
        case StateInfo::NEW: {
            push_state(succ_id, succ_info, value_store[succ_id]);
            return true; // recursion on new state
        }

        case StateInfo::CLOSED: {
            const AlgorithmValueType value = value_store[succ_id];
            explore.q_value.conv_part += prob * value;
            explore.exit_interval.lower =
                std::min(explore.exit_interval.lower, value);
            explore.exit_interval.upper =
                std::max(explore.exit_interval.upper, value);
            break;
        }

        case StateInfo::ONSTACK:
            unsigned succ_stack_id = succ_info.stack_id;
            explore.lowlink = std::min(explore.lowlink, succ_stack_id);
            explore.q_value.scc_successors.emplace_back(succ_id, prob);

            auto& parents = stack_[succ_stack_id].parents;
            parents.emplace_back(
                explore.stackidx,
                explore.stack_info.transition_flags.size());
        }
    } while (explore.next_successor() || explore.next_transition(mdp));

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::initialize_state(
    MDPType& mdp,
    const HeuristicType& heuristic,
    ExplorationInfo& exp_info,
    auto& value_store)
{
    assert(
        state_information_[exp_info.state_id].get_status() ==
        StateInfo::ONSTACK);

    TimerScope _(statistics_.initialize_state_timer);

    const State state = mdp.get_state(exp_info.state_id);

    const TerminationInfo state_term = mdp.get_termination_info(state);
    const value_t t_cost = state_term.get_cost();
    const value_t estimate = heuristic.evaluate(state);

    exp_info.stack_info.conv_part = AlgorithmValueType(t_cost);
    exp_info.exit_interval = Interval(t_cost);

    AlgorithmValueType& state_value = value_store[exp_info.state_id];

    if constexpr (UseInterval) {
        state_value.lower = estimate;
        state_value.upper = t_cost;
    } else {
        state_value = estimate;
    }

    if (t_cost != INFINITE_VALUE) {
        ++exp_info.stack_info.active_exit_transitions;
        ++exp_info.stack_info.active_transitions;
    }

    if (state_term.is_goal_state()) {
        ++statistics_.goal_states;
    } else if (estimate == t_cost) {
        ++statistics_.pruned;
        return false;
    }

    mdp.generate_applicable_actions(state, exp_info.aops);

    const size_t num_aops = exp_info.aops.size();

    exp_info.stack_info.ec_transitions.reserve(num_aops);

    ++statistics_.expanded_states;

    if (exp_info.aops.empty()) {
        ++statistics_.terminal_states;
    } else if (exp_info.forward_non_loop_transition(mdp, state)) {
        return true;
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::scc_found(
    auto& value_store,
    ExplorationInfo& exp_info,
    unsigned int stack_idx,
    utils::CountdownTimer& timer)
{
    using namespace std::views;

    TimerScope _(statistics_.scc_handling_timer);

    auto scc = stack_ | drop(stack_idx);

    assert(!scc.empty());

    ++statistics_.sccs;

    if (exp_info.exit_interval.lower == INFINITE_VALUE ||
        (exp_info.exit_interval.lower == exp_info.exit_interval.upper &&
         exp_info.has_all_zero)) {
        for (StackInfo& stk_info : scc) {
            StateInfo& state_info = state_information_[stk_info.state_id];
            assert(state_info.get_status() == StateInfo::ONSTACK);
            *stk_info.value = exp_info.exit_interval.lower;
            state_info.stack_id = StateInfo::UNDEF;
        }

        stack_.erase(scc.begin(), scc.end());
        return;
    }

    if (scc.size() == 1) {
        // For singleton SCCs, we only have transitions which are
        // self-loops or go to a state that is topologically greater.
        // The state value is therefore the base value.
        StackInfo& single = scc.front();
        StateInfo& state_info = state_information_[single.state_id];
        assert(state_info.get_status() == StateInfo::ONSTACK);
        *single.value = single.conv_part;
        state_info.stack_id = StateInfo::UNDEF;
        ++statistics_.singleton_sccs;
        stack_.pop_back();
        return;
    }

    if (std::ranges::any_of(scc, [](const StackInfo& stk_info) {
            return !stk_info.non_ec_transitions.empty();
        })) {
        // Run recursive EC Decomposition
        TimerScope _(statistics_.decomposition_timer);

        scc_.reserve(scc.size());

        for (const auto state_id : scc | transform(&StackInfo::state_id)) {
            scc_.push_back(state_id);
        }

        // Run decomposition
        find_and_decompose_sccs(timer);

        for (StackInfo& stack_info : scc) {
            StateInfo& state_info = state_information_[stack_info.state_id];
            state_info.stack_id = StateInfo::UNDEF;
            assert(state_info.get_status() == StateInfo::CLOSED);
        }

        assert(exploration_stack_ecd_.empty());
    } else {
        // We found an end component, patch it
        StackInfo& repr_stk = scc.front();
        const StateID scc_repr_id = repr_stk.state_id;
        state_information_[scc_repr_id].stack_id = StateInfo::UNDEF;

        // Spider construction
        for (StackInfo& succ_stk : scc | drop(1)) {
            state_information_[succ_stk.state_id].stack_id = StateInfo::UNDEF;

            // Move all non-EC transitions to representative state
            auto& tr = succ_stk.non_ec_transitions;
            for (auto it = tr.begin(); it != tr.end();) {
                repr_stk.add_non_ec_transition(
                    std::move(tr.extract(it++).value()));
            }

            // Free memory
            std::decay_t<decltype(tr)>().swap(tr);

            set_min(repr_stk.conv_part, succ_stk.conv_part);

            succ_stk.conv_part = AlgorithmValueType(INFINITE_VALUE);

            // Connect to representative state with zero cost action
            succ_stk.non_ec_transitions.emplace(
                0_vt,
                1_vt,
                std::vector{ItemProbabilityPair<StateID>(scc_repr_id, 1_vt)});
        }
    }

    class Partition {
        std::vector<std::vector<int>::iterator> scc_index_to_local;
        std::vector<int> partition;
        std::vector<int>::iterator solvable_beg;
        std::vector<int>::iterator solvable_exits_beg;

    public:
        explicit Partition(std::size_t size)
            : scc_index_to_local(size)
            , partition(size, 0)
        {
            for (unsigned int i = 0; i != size; ++i) {
                scc_index_to_local[i] = partition.begin() + i;
                partition[i] = static_cast<int>(i);
            }

            solvable_beg = partition.begin();
            solvable_exits_beg = partition.begin();
        }

        auto solvable_begin() { return solvable_beg; }
        auto solvable_end() { return partition.end(); }

        auto solvable()
        {
            return std::ranges::subrange(solvable_begin(), solvable_end());
        }

        [[nodiscard]]
        bool has_solvable() const
        {
            return solvable_beg != partition.end();
        }

        void demote_unsolvable(int s)
        {
            assert(scc_index_to_local[s] >= solvable_beg);
            assert(scc_index_to_local[s] < solvable_exits_beg);

            auto local = scc_index_to_local[s];
            std::swap(scc_index_to_local[*solvable_beg], scc_index_to_local[s]);
            std::swap(*solvable_beg, *local);

            ++solvable_beg;

            assert(scc_index_to_local[s] < solvable_beg);
        }

        void demote_exit_unsolvable(int s)
        {
            demote_exit_solvable(s);
            demote_unsolvable(s);
        }

        void demote_exit_solvable(int s)
        {
            assert(scc_index_to_local[s] >= solvable_exits_beg);

            auto local = scc_index_to_local[s];
            std::swap(
                scc_index_to_local[*solvable_exits_beg],
                scc_index_to_local[s]);
            std::swap(*solvable_exits_beg, *local);

            ++solvable_exits_beg;

            assert(scc_index_to_local[s] >= solvable_beg);
            assert(scc_index_to_local[s] < solvable_exits_beg);
        }

        bool promote_solvable(int s)
        {
            if (!is_unsolvable(s)) {
                return false;
            }

            --solvable_beg;

            auto local = scc_index_to_local[s];
            std::swap(scc_index_to_local[*solvable_beg], scc_index_to_local[s]);
            std::swap(*solvable_beg, *local);

            assert(scc_index_to_local[s] >= solvable_beg);
            assert(scc_index_to_local[s] < solvable_exits_beg);

            return true;
        }

        void mark_non_exit_states_unsolvable()
        {
            solvable_beg = solvable_exits_beg;
        }

        bool is_unsolvable(int s)
        {
            return scc_index_to_local[s] < solvable_beg;
        }
    };

    {
        TimerScope _(statistics_.solvability_timer);

        // Set the value of unsolvable states of this SCC to -inf.
        // Start by partitioning states into inactive states, active exits and
        // active non-exists.
        // The partition is initialized optimistically, i.e., all states start
        // out as active exits.
        Partition partition(scc.size());

        for (std::size_t i = 0; i != scc.size(); ++i) {
            StackInfo& info = scc[i];

            assert(
                info.active_transitions != 0 ||
                info.active_exit_transitions == 0);

            // Transform to local indices
            for (auto& parent_info : info.parents) {
                parent_info.parent_idx -= stack_idx;
            }

            if (info.active_exit_transitions == 0) {
                if (info.active_transitions > 0) {
                    partition.demote_exit_solvable(i);
                } else {
                    value_store[info.state_id] =
                        AlgorithmValueType(INFINITE_VALUE);
                    partition.demote_exit_unsolvable(i);
                }
            }
        }

        if (partition.has_solvable()) {
            // Compute the set of solvable states of this SCC.
            for (;;) {
                timer.throw_if_expired();

                // Collect states that can currently reach an exit and mark
                // other states unsolvable.
                auto unsolv_it = partition.solvable_begin();

                partition.mark_non_exit_states_unsolvable();

                for (auto it = partition.solvable_end();
                     it != partition.solvable_begin();) {
                    for (const auto& [parent_idx, tr_idx] :
                         scc[*--it].parents) {
                        StackInfo& pinfo = scc[parent_idx];

                        if (pinfo.transition_flags[tr_idx].is_active) {
                            partition.promote_solvable(parent_idx);
                        }
                    }
                }

                // No new unsolvable states -> stop.
                if (unsolv_it == partition.solvable_begin()) break;

                // Run fixpoint iteration starting with the new unsolvable
                // states that could not reach an exit anymore.
                do {
                    timer.throw_if_expired();

                    StackInfo& scc_elem = scc[*unsolv_it];

                    // The state was marked unsolvable.
                    assert(partition.is_unsolvable(*unsolv_it));

                    value_store[scc_elem.state_id] =
                        AlgorithmValueType(INFINITE_VALUE);

                    for (const auto& [parent_idx, tr_idx] : scc_elem.parents) {
                        StackInfo& pinfo = scc[parent_idx];
                        auto& transition_flags = pinfo.transition_flags[tr_idx];

                        assert(
                            !transition_flags.is_active_exiting ||
                            transition_flags.is_active);

                        if (partition.is_unsolvable(parent_idx)) continue;

                        if (transition_flags.is_active_exiting) {
                            transition_flags.is_active_exiting = false;
                            transition_flags.is_active = false;

                            --pinfo.active_transitions;
                            --pinfo.active_exit_transitions;

                            if (pinfo.active_transitions == 0) {
                                partition.demote_exit_unsolvable(parent_idx);
                            } else if (pinfo.active_exit_transitions == 0) {
                                partition.demote_exit_solvable(parent_idx);
                            }
                        } else if (transition_flags.is_active) {
                            transition_flags.is_active = false;

                            --pinfo.active_transitions;

                            if (pinfo.active_transitions == 0) {
                                partition.demote_unsolvable(parent_idx);
                            }
                        }
                    }
                } while (++unsolv_it != partition.solvable_begin());
            }
        }
    }

    // Now run VI on the SCC until convergence
    {
        TimerScope _(statistics_.vi_timer);

        struct VIInfo {
            AlgorithmValueType* value;
            AlgorithmValueType conv_part;
            std::vector<QValueInfo> transitions;
        };

        std::vector<VIInfo> table;
        table.reserve(scc.size());

        for (auto it = scc.begin(); it != scc.end(); ++it) {
            auto& t = table.emplace_back(it->value, it->conv_part);
            auto& tr = it->non_ec_transitions;
            t.transitions.reserve(tr.size());
            for (auto it2 = tr.begin(); it2 != tr.end();) {
                t.transitions.push_back(std::move(tr.extract(it2++).value()));
            }
        }

        bool converged;

        do {
            timer.throw_if_expired();

            converged = true;
            auto it = table.begin();

            do {
                AlgorithmValueType v = it->conv_part;

                for (const QValueInfo& info : it->transitions) {
                    set_min(v, info.compute_q_value(value_store));
                }

                if constexpr (UseInterval) {
                    *it->value = v;
                    if (!it->value->bounds_equal()) converged = false;
                } else {
                    if (update(*it->value, v, this->epsilon)) converged = false;
                }

                ++statistics_.bellman_backups;
            } while (++it != table.end());
        } while (!converged);
    }

    stack_.erase(scc.begin(), scc.end());
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::
    find_and_decompose_sccs(utils::CountdownTimer& timer)
{
    do {
        for (const StateID state_id : scc_) {
            state_information_[state_id].explored = 0;
        }

        for (const StateID state_id : scc_) {
            StateInfo& state_info = state_information_[state_id];
            if (state_info.get_ecd_status() != StateInfo::NEW) continue;

            state_info.explored = 1;
            state_info.ecd_stack_id = 0;
            exploration_stack_ecd_.emplace_back(stack_[state_info.stack_id], 0);
            stack_ecd_.emplace_back(state_id);

            for (;;) {
                ECDExplorationInfo* e;

                // DFS recursion
                do {
                    e = &exploration_stack_ecd_.back();
                } while (initialize_ecd(*e) && push_successor_ecd(*e, timer));

                // Iterative backtracking
                do {
                    const unsigned int stck = e->stackidx;
                    const unsigned int lowlink = e->lowlink;

                    assert(stck >= lowlink);

                    const bool backtracked_from_scc = stck == lowlink;

                    if (backtracked_from_scc) {
                        scc_found_ecd(*e);

                        if (stck == 0) {
                            assert(stack_ecd_.empty());
                            assert(exploration_stack_ecd_.size() == 1);
                            exploration_stack_ecd_.pop_back();
                            goto break_outer;
                        }
                    }

                    assert(exploration_stack_ecd_.size() > 1);

                    timer.throw_if_expired();

                    const ECDExplorationInfo& successor = *e--;

                    if (backtracked_from_scc) {
                        e->leaves_scc = true;
                    } else {
                        e->lowlink = std::min(e->lowlink, lowlink);
                        e->recurse = e->recurse || successor.recurse;
                        e->remains_scc = true;
                    }

                    exploration_stack_ecd_.pop_back();
                } while ((!e->next_successor() && !e->next_transition()) ||
                         !push_successor_ecd(*e, timer));
            }

        break_outer:;
        }

        scc_.clear();
    } while (decomposition_queue_.pop_scc(scc_));

    assert(scc_.empty());
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    push_successor_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ_id = e.get_current_successor().item;
        StateInfo& succ_info = state_information_[succ_id];

        switch (succ_info.get_ecd_status()) {
        case StateInfo::NEW: {
            const auto stack_size = stack_ecd_.size();
            succ_info.explored = 1;
            succ_info.ecd_stack_id = stack_size;
            exploration_stack_ecd_.emplace_back(
                stack_[succ_info.stack_id],
                stack_size);
            stack_ecd_.emplace_back(succ_id);
            return true;
        }

        case StateInfo::CLOSED: e.leaves_scc = true; break;

        case StateInfo::ONSTACK:
            e.lowlink = std::min(e.lowlink, succ_info.ecd_stack_id);
            e.remains_scc = true;
        }
    } while (e.next_successor() || e.next_transition());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::initialize_ecd(
    ECDExplorationInfo& exp_info)
{
    StackInfo& stack_info = exp_info.stack_info;

    if (stack_info.ec_transitions.empty()) {
        return false;
    }

    exp_info.action = stack_info.ec_transitions.begin();
    exp_info.end = stack_info.ec_transitions.end();
    exp_info.successor = exp_info.action->scc_successors.begin();

    return true;
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::scc_found_ecd(
    ECDExplorationInfo& e)
{
    namespace vws = std::views;

    auto scc = stack_ecd_ | std::views::drop(e.stackidx);

    if (scc.size() == 1) {
        state_information_[scc.front()].ecd_stack_id = StateInfo::UNDEF_ECD;
    } else if (e.recurse) {
        decomposition_queue_.register_new_scc();
        for (const StateID state_id : scc) {
            decomposition_queue_.add_scc_state(state_id);
            state_information_[state_id].ecd_stack_id = StateInfo::UNDEF_ECD;
        }
    } else {
        // We found an end component, patch it
        const StateID scc_repr_id = scc.front();
        StateInfo& repr_state_info = state_information_[scc_repr_id];
        StackInfo& repr_stk = stack_[repr_state_info.stack_id];

        repr_state_info.ecd_stack_id = StateInfo::UNDEF_ECD;

        // Spider construction
        for (const StateID state_id : scc | std::views::drop(1)) {
            StateInfo& state_info = state_information_[state_id];
            StackInfo& succ_stk = stack_[state_info.stack_id];

            state_info.ecd_stack_id = StateInfo::UNDEF_ECD;

            // Move all non-EC transitions to representative state
            auto& tr = succ_stk.non_ec_transitions;
            for (auto it = tr.begin(); it != tr.end();) {
                repr_stk.add_non_ec_transition(
                    std::move(tr.extract(it++).value()));
            }

            // Free memory
            std::decay_t<decltype(tr)>().swap(tr);

            set_min(repr_stk.conv_part, succ_stk.conv_part);
            succ_stk.conv_part = AlgorithmValueType(INFINITE_VALUE);

            // Connect to representative state with zero cost action
            succ_stk.non_ec_transitions.emplace(
                0_vt,
                1_vt,
                std::vector{ItemProbabilityPair<StateID>(scc_repr_id, 1_vt)});
        }
    }

    stack_ecd_.erase(scc.begin(), scc.end());

    assert(stack_ecd_.size() == e.stackidx);
}

} // namespace probfd::algorithms::ta_topological_vi
