#ifndef PROBFD_ENGINES_FRET_H
#define PROBFD_ENGINES_FRET_H

#include "probfd/engines/engine.h"
#include "probfd/engines/heuristic_search_base.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/progress_report.h"
#include "probfd/quotients/quotient_system.h"

#if defined(EXPENSIVE_STATISTICS)
#include "../../utils/timer.h"
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

    Interval solve(const State& state) override
    {
        for (;;) {
            const Interval value = heuristic_search(state);

            if (find_and_remove_traps(state)) {
                return value;
            }

            base_engine_->reset_search_state();
        }
    }

    Interval heuristic_search(const State& state)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope scoped(statistics_.heuristic_search);
#endif
        return base_engine_->solve(state);
    }

    void print_statistics(std::ostream& out) const override
    {
        this->base_engine_->print_statistics(out);
        statistics_.print(out);
    }

private:
    bool find_and_remove_traps(const State& state)
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
        std::vector<std::vector<Action>> ops;
        ops.reserve(std::distance(first, last));

        for (auto it = first; it != last; ++it) {
            std::vector<QAction> qops;
            greedy_graph.get_actions(*base_engine_, *it, qops);
            auto& orig_ops = ops.emplace_back();
            for (const QAction& qop : qops) {
                orig_ops.push_back(quotient_->get_original_action(*it, qop));
            }
        }

        // Now collapse the quotient
        quotient_->build_quotient(first, last, repr, ops.begin());
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

        bool value_changed =
            base_engine.compute_value_update_and_optimal_transitions(
                qstate,
                opt_aops,
                opt_transitions);

        for (const auto& transition : opt_transitions) {
            for (const StateID sid : transition.elements()) {
                if (ids.insert(sid).second) {
                    successors.push_back(sid);
                }
            }
        }

        ids.clear();
        opt_aops.clear();
        opt_transitions.clear();

        return value_changed;
    }

    void get_actions(
        HeuristicSearchEngine<State, QAction, UseInterval>& base_engine,
        StateID qstate,
        std::vector<QAction>& aops)
    {
        assert(opt_transitions.empty());

        base_engine.compute_value_update_and_optimal_transitions(
            qstate,
            aops,
            opt_transitions);

        opt_transitions.clear();
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
        t_.clear();
        bool result = base_engine.apply_policy(qstate, t_);
        for (StateID sid : t_.elements()) {
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