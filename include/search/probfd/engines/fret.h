#ifndef PROBFD_ENGINES_FRET_H
#define PROBFD_ENGINES_FRET_H

#include "probfd/engines/engine.h"
#include "probfd/engines/heuristic_search_base.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/progress_report.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/policies/map_policy.h"

#include "probfd/task_utils/task_properties.h"

#include "utils/countdown_timer.h"

#if defined(EXPENSIVE_STATISTICS)
#include "utils/timer.h"
#endif

#include <fstream>
#include <memory>
#include <sstream>
#include <type_traits>

namespace probfd {
namespace engines {

/// Namespace dedicated to the Find, Revise, Eliminate Traps (FRET) framework.
namespace fret {

template <typename State, typename Action, bool UseInterval>
using HeuristicSearchEngine =
    heuristic_search::HeuristicSearchBase<State, Action, UseInterval, true>;

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long traps = 0;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer heuristic_search = utils::Timer(true);
    utils::Timer trap_identification = utils::Timer(true);
    utils::Timer trap_removal = utils::Timer(true);
#endif

    void print(std::ostream& out) const
    {
        out << "  FRET iterations: " << iterations << std::endl;
#if defined(EXPENSIVE_STATISTICS)
        out << "  Heuristic search: " << heuristic_search << std::endl;
        out << "  Trap identification: "
            << (trap_identification() - trap_removal()) << std::endl;
        out << "  Trap removal: " << trap_removal << std::endl;
#endif
    }
};

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
class FRET : public MDPEngine<State, Action> {
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = typename QuotientSystem::QAction;

    struct TarjanStateInformation {
        static constexpr unsigned UNDEF = -1;

        unsigned stack_index = UNDEF;
        unsigned lowlink = UNDEF;

        bool is_explored() const { return lowlink != UNDEF; }
        bool is_on_stack() const { return stack_index != UNDEF; }

        void open(const unsigned x)
        {
            stack_index = x;
            lowlink = x;
        }

        void close() { stack_index = UNDEF; }
    };

    struct ExplorationInfo {
        ExplorationInfo(StateID state_id, std::vector<StateID> successors)
            : state_id(state_id)
            , successors(std::move(successors))
        {
        }

        StateID state_id;
        std::vector<StateID> successors;
        bool is_leaf = true;
    };

    QuotientSystem* quotient_;
    std::shared_ptr<HeuristicSearchEngine<State, QAction, UseInterval>>
        base_engine_;

    Statistics statistics_;

public:
    FRET(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        QuotientSystem* quotient,
        ProgressReport* report,
        std::shared_ptr<HeuristicSearchEngine<State, QAction, UseInterval>>
            engine)
        : MDPEngine<State, Action>(state_space, cost_function)
        , quotient_(quotient)
        , base_engine_(engine)
    {
        report->register_print([&](std::ostream& out) {
            out << "fret=" << statistics_.iterations
                << ", traps=" << statistics_.traps;
        });
    }

    std::unique_ptr<PartialPolicy<State, Action>>
    compute_policy(const State& state, double max_time) override
    {
        this->solve(state, max_time);

        /*
         * The quotient policy only specifies the optimal actions between traps.
         * We need to supplement the optimal actions within the traps, i.e.
         * the actions which point every other member state of the trap towards
         * that trap member state that owns the optimal quotient action.
         *
         * We fully explore the quotient policy starting from the initial state
         * and compute the optimal 'inner' actions for each trap. To this end,
         * we first generate the sub-MDP of the trap. Afterwards, we expand the
         * trap graph backwards from the state that has the optimal quotient
         * action. For each encountered state, we select the action with which
         * it is encountered first as the policy action.
         */

        std::unique_ptr<policies::MapPolicy<State, Action>> policy(
            new policies::MapPolicy<State, Action>(this->get_state_space()));

        const StateID initial_state_id = this->get_state_id(state);

        std::deque<StateID> queue;
        std::set<StateID> visited;
        queue.push_back(initial_state_id);
        visited.insert(initial_state_id);

        do {
            const StateID quotient_id = queue.front();
            queue.pop_front();

            ActionID quotient_action_id =
                base_engine_->lookup_policy(quotient_id);

            // Terminal states have no policy decision.
            if (quotient_action_id == ActionID::undefined) {
                continue;
            }

            const QAction quotient_action =
                base_engine_->lookup_action(quotient_id, quotient_action_id);
            const Interval quotient_bound =
                base_engine_->lookup_dual_bounds(quotient_id);

            const StateID exiting_id = quotient_action.state_id;

            policy->emplace_decision(
                exiting_id,
                quotient_action.action_id,
                quotient_bound);

            // Nothing else needs to be done if the trap has only one state.
            if (quotient_->quotient_size(quotient_id) != 1) {
                std::unordered_map<
                    StateID,
                    std::set<std::pair<StateID, ActionID>>>
                    parents;

                // Build the inverse graph
                std::vector<QAction> inner_actions;
                quotient_->get_pruned_ops(quotient_id, inner_actions);

                for (const QAction& qaction : inner_actions) {
                    StateID source_id = qaction.state_id;
                    ActionID action_id = qaction.action_id;

                    Distribution<StateID> successors;
                    this->generate_successors(
                        source_id,
                        this->lookup_action(source_id, action_id),
                        successors);

                    for (const StateID succ_id : successors.support()) {
                        parents[succ_id].insert({source_id, action_id});
                    }
                }

                // Now traverse the inverse graph starting from the exiting
                // state
                std::deque<StateID> inverse_queue;
                std::set<StateID> inverse_visited;
                inverse_queue.push_back(exiting_id);
                inverse_visited.insert(exiting_id);

                do {
                    const StateID next_id = inverse_queue.front();
                    inverse_queue.pop_front();

                    for (const auto& [pred_id, act_id] : parents[next_id]) {
                        if (inverse_visited.insert(pred_id).second) {
                            policy->emplace_decision(
                                pred_id,
                                act_id,
                                quotient_bound);
                            inverse_queue.push_back(pred_id);
                        }
                    }
                } while (!inverse_queue.empty());
            }

            // Push the successor traps.
            Distribution<StateID> successors;
            quotient_->generate_action_transitions(
                quotient_id,
                quotient_action,
                successors);

            for (const StateID succ_id : successors.support()) {
                if (visited.insert(succ_id).second) {
                    queue.push_back(succ_id);
                }
            }
        } while (!queue.empty());

        return policy;
    }

    Interval solve(const State& state, double max_time) override
    {
        utils::CountdownTimer timer(max_time);

        for (;;) {
            const Interval value = heuristic_search(state, timer);

            if (find_and_remove_traps(state, timer)) {
                return value;
            }

            base_engine_->reset_search_state();
        }
    }

    Interval heuristic_search(const State& state, utils::CountdownTimer& timer)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope scoped(statistics_.heuristic_search);
#endif
        return base_engine_->solve(state, timer.get_remaining_time());
    }

    void print_statistics(std::ostream& out) const override
    {
        this->base_engine_->print_statistics(out);
        statistics_.print(out);
    }

private:
    bool find_and_remove_traps(const State& state, utils::CountdownTimer&)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope scoped(statistics_.trap_identification);
#endif
        unsigned int trap_counter = 0;
        unsigned int unexpanded = 0;

        storage::StateHashMap<TarjanStateInformation> state_infos;
        std::deque<ExplorationInfo> exploration_queue;
        std::deque<StateID> stack;

        {
            StateID stateid = this->get_state_id(state);
            push(exploration_queue, stateid, unexpanded);
            stack.push_back(stateid);
            state_infos[stateid].open(0);
        }

        bool backtracked = false;
        bool can_reach_child_scc = false;
        unsigned last_lowlink = TarjanStateInformation::UNDEF;

        while (!exploration_queue.empty()) {
            ExplorationInfo& einfo = exploration_queue.back();
            const auto state_id = einfo.state_id;
            TarjanStateInformation& sinfo = state_infos[state_id];

            if (backtracked) {
                sinfo.lowlink = std::min(sinfo.lowlink, last_lowlink);
                if (can_reach_child_scc) {
                    einfo.is_leaf = false;
                }
            }

            while (!einfo.successors.empty()) {
                StateID succid = einfo.successors.back();
                einfo.successors.pop_back();
                TarjanStateInformation& succ_info = state_infos[succid];

                if (!succ_info.is_explored()) {
                    if (push(exploration_queue, succid, unexpanded)) {
                        succ_info.open(stack.size());
                        stack.push_back(succid);
                        backtracked = false;
                        goto continue_outer;
                    }

                    einfo.is_leaf = false;
                } else if (succ_info.is_on_stack()) {
                    sinfo.lowlink =
                        std::min(sinfo.lowlink, succ_info.stack_index);
                } else {
                    einfo.is_leaf = false;
                }
            }

            backtracked = true;
            can_reach_child_scc = !einfo.is_leaf;
            last_lowlink = sinfo.lowlink;

            if (sinfo.lowlink == sinfo.stack_index) {
                can_reach_child_scc = true;

                auto scc_begin = stack.begin() + sinfo.stack_index;
                auto scc_end = stack.end();

                for (auto it = scc_begin; it != scc_end; ++it) {
                    state_infos[*it].close();
                }

                if (einfo.is_leaf) {
                    // Terminal and self-loop leaf SCCs are always pruned
                    assert(std::distance(scc_begin, scc_end) > 1);
                    collapse_trap(scc_begin, scc_end, state_id);
                    base_engine_->async_update(state_id);
                    ++trap_counter;
                }

                stack.erase(scc_begin, scc_end);
            }

            exploration_queue.pop_back();

        continue_outer:;
        }

        ++statistics_.iterations;

        return trap_counter == 0 && unexpanded == 0;
    }

    using StackIterator = std::deque<StateID>::iterator;

    void collapse_trap(StackIterator first, StackIterator last, StateID repr)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope t(statistics_.trap_removal);
#endif

        GreedyGraphGenerator greedy_graph;

        // Get the greedy transitions to collapse them
        std::vector<std::vector<QAction>> qops;
        qops.reserve(std::distance(first, last));
        for (auto it = first; it != last; ++it) {
            greedy_graph.get_actions(*base_engine_, *it, qops.emplace_back());
        }

        // Now collapse the quotient
        quotient_->build_quotient(first, last, repr, qops.begin());
        base_engine_->clear_policy(repr);

        ++statistics_.traps;
    }

    bool push(
        std::deque<ExplorationInfo>& queue,
        StateID state_id,
        unsigned int& unexpanded)
    {
        if (base_engine_->is_terminal(state_id)) {
            return false;
        }

        GreedyGraphGenerator greedy_graph;
        std::vector<StateID> succs;
        if (greedy_graph.get_successors(*base_engine_, state_id, succs)) {
            ++unexpanded;
        }

        if (succs.empty()) {
            return false;
        }

        queue.emplace_back(state_id, std::move(succs));
        return true;
    }
};

template <typename State, typename Action, bool UseInterval>
class ValueGraph {
    using QAction = typename quotients::QuotientSystem<State, Action>::QAction;

    std::unordered_set<StateID> ids;
    std::vector<QAction> opt_aops;
    std::vector<Distribution<StateID>> opt_transitions;

public:
    bool get_successors(
        HeuristicSearchEngine<State, QAction, UseInterval>& base_engine,
        StateID qstate,
        std::vector<StateID>& successors)
    {
        assert(successors.empty());

        ClearGuard _guard(ids, opt_aops, opt_transitions);

        bool value_changed =
            base_engine.compute_value_update_and_optimal_transitions(
                qstate,
                opt_aops,
                opt_transitions);

        for (const auto& transition : opt_transitions) {
            for (const StateID sid : transition.support()) {
                if (ids.insert(sid).second) {
                    successors.push_back(sid);
                }
            }
        }

        return value_changed;
    }

    void get_actions(
        HeuristicSearchEngine<State, QAction, UseInterval>& base_engine,
        StateID qstate,
        std::vector<QAction>& aops)
    {
        ClearGuard _guard(opt_transitions);

        base_engine.compute_value_update_and_optimal_transitions(
            qstate,
            aops,
            opt_transitions);
    }
};

template <typename State, typename Action, bool UseInterval>
class PolicyGraph {
    using QAction = typename quotients::QuotientSystem<State, Action>::QAction;

    Distribution<StateID> t_;

public:
    bool get_successors(
        HeuristicSearchEngine<State, QAction, UseInterval>& base_engine,
        StateID qstate,
        std::vector<StateID>& successors)
    {
        ClearGuard _guard(t_);

        bool result = base_engine.apply_policy(qstate, t_);
        for (StateID sid : t_.support()) {
            successors.push_back(sid);
        }
        return result;
    }

    void get_actions(
        HeuristicSearchEngine<State, QAction, UseInterval>& base_engine,
        StateID qstate,
        std::vector<QAction>& aops)
    {
        assert(aops.empty());
        aops.emplace_back(base_engine.get_policy(qstate));
    }
};

} // namespace internal

template <typename State, typename Action, bool UseInterval>
using FRETV = internal::FRET<
    State,
    Action,
    UseInterval,
    typename internal::ValueGraph<State, Action, UseInterval>>;

template <typename State, typename Action, bool UseInterval>
using FRETPi = internal::FRET<
    State,
    Action,
    UseInterval,
    typename internal::PolicyGraph<State, Action, UseInterval>>;

} // namespace fret
} // namespace engines
} // namespace probfd

#endif // __FRET_H__