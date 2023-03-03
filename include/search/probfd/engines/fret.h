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

template <typename State, typename Action, bool Interval>
using HeuristicSearchEngine =
    heuristic_search::HeuristicSearchBase<State, Action, Interval, true>;

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long traps = 0;
    unsigned long long traps_dead = 0;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer heuristic_search = utils::Timer(true);
    utils::Timer trap_identification = utils::Timer(true);
    utils::Timer trap_removal = utils::Timer(true);
#endif

    void print(std::ostream& out) const
    {
        out << "  FRET iterations: " << iterations << std::endl;
        out << "  Permanent traps: " << traps << " (" << traps_dead << " dead)"
            << std::endl;
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
    bool Interval,
    typename GreedyGraphGenerator>
class FRET : public MDPEngine<State, Action> {
    struct TarjanStateInformation {
        static constexpr unsigned UNDEF = -1;

        void open(const unsigned x)
        {
            onstack = true;
            index = x;
            lowlink = x;
        }

        unsigned index = UNDEF;
        unsigned lowlink = UNDEF;
        bool onstack = false;
    };

    struct ExplorationInfo {
        ExplorationInfo(StateID state_id)
            : state_id(state_id)
        {
        }

        StateID state_id;
        std::vector<StateID> successors;
        bool is_leaf = true;
    };

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = typename QuotientSystem::QAction;

    GreedyGraphGenerator greedy_graph_;
    ProgressReport* report_;
    QuotientSystem* quotient_;
    std::shared_ptr<HeuristicSearchEngine<State, QAction, Interval>>
        base_engine_;

    Statistics statistics_;
    bool last_dead_;
    unsigned trap_counter_;
    unsigned unexpanded_;

public:
    FRET(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        QuotientSystem* quotient,
        ProgressReport* report,
        std::shared_ptr<HeuristicSearchEngine<State, QAction, Interval>> engine)
        : MDPEngine<State, Action>(state_space, cost_function)
        , greedy_graph_(engine.get())
        , report_(report)
        , quotient_(quotient)
        , base_engine_(engine)
    {
    }

    value_t solve(const State& state) override
    {
        report_->register_print([&](std::ostream& out) {
            out << "fret=" << statistics_.iterations
                << ", traps=" << statistics_.traps;
        });

        for (;;) {
            value_t value = heuristic_search(state);

            find_and_remove_traps(state);

            ++statistics_.iterations;

            if (trap_counter_ + unexpanded_ == 0) {
                return value;
            }

            base_engine_->reset_search_state();
        }
    }

    value_t heuristic_search(const State& state)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope scoped(statistics_.heuristic_search);
#endif
        return base_engine_->solve(state);
    }

    std::optional<value_t> get_error(const State& state) override
    {
        return base_engine_->get_error(state);
    }

    void print_statistics(std::ostream& out) const override
    {
        this->base_engine_->print_statistics(out);
        statistics_.print(out);
    }

private:
    void find_and_remove_traps(const State& state)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope scoped(statistics_.trap_identification);
#endif
        trap_counter_ = 0;
        unexpanded_ = 0;

        storage::StateHashMap<TarjanStateInformation> state_infos;
        std::deque<ExplorationInfo> exploration_queue;
        std::deque<StateID> stack;
        unsigned current_index = 0;

        {
            StateID stateid = this->get_state_id(state);
            push(exploration_queue, stateid);
            stack.push_back(stateid);
            state_infos[stateid].open(current_index);
        }

        bool backtracked = false;
        unsigned last_lowlink = TarjanStateInformation::UNDEF;

        while (!exploration_queue.empty()) {
            ExplorationInfo& einfo = exploration_queue.back();
            const auto state_id = einfo.state_id;
            TarjanStateInformation& sinfo = state_infos[state_id];
            sinfo.lowlink = std::min(sinfo.lowlink, last_lowlink);
            einfo.is_leaf = einfo.is_leaf && !backtracked;
            bool fully_explored = true;

            while (!einfo.successors.empty()) {
                StateID succid = einfo.successors.back();
                einfo.successors.pop_back();
                TarjanStateInformation& succ_info = state_infos[succid];

                if (succ_info.index == TarjanStateInformation::UNDEF) {
                    if (push(exploration_queue, succid)) {
                        succ_info.open(++current_index);
                        stack.push_front(succid);
                        backtracked = false;
                        last_lowlink = TarjanStateInformation::UNDEF;
                        fully_explored = false;
                        break;
                    }

                    einfo.is_leaf = false;
                } else if (succ_info.onstack) {
                    sinfo.lowlink = std::min(sinfo.lowlink, succ_info.index);
                } else {
                    einfo.is_leaf = false;
                }
            }

            if (fully_explored) {
                backtracked = !einfo.is_leaf;
                last_lowlink = sinfo.lowlink;

                if (sinfo.lowlink == sinfo.index) {
                    backtracked = true;
                    unsigned scc_size = 0;

                    auto it = stack.begin();
                    do {
                        state_infos[*it].onstack = false;
                        ++scc_size;
                    } while (*(it++) != state_id);

                    if (einfo.is_leaf) {
                        if (scc_size > 1) {
                            trap_counter_++;
                            collapse_trap(stack.begin(), it, state_id);
                            base_engine_->async_update(state_id);
                            statistics_.traps++;
                        } else {
                            base_engine_->notify_dead_end_ifnot_goal(state_id);
                        }
                    }

                    stack.erase(stack.begin(), it);
                }

                exploration_queue.pop_back();
            }
        }
    }

    using StackIterator = std::deque<StateID>::iterator;

    void collapse_trap(StackIterator first, StackIterator last, StateID repr)
    {
#if defined(EXPENSIVE_STATISTICS)
        utils::TimerScope t(statistics_.trap_removal);
#endif
        quotient_->build_quotient(first, last, repr);
        base_engine_->clear_policy(repr);
    }

    bool push(std::deque<ExplorationInfo>& queue, StateID state_id)
    {
        if (base_engine_->is_terminal(state_id)) {
            return false;
        }

        std::vector<StateID> succs;
        if (greedy_graph_(state_id, succs)) {
            ++unexpanded_;
        }

        if (succs.empty()) {
            return false;
        }

        queue.emplace_back(state_id).successors.swap(succs);
        return true;
    }
};

template <typename State, typename Action, bool Interval>
class ValueGraph {
    using QAction = typename quotients::QuotientSystem<State, Action>::QAction;

    HeuristicSearchEngine<State, QAction, Interval>* base_engine_;

    std::unordered_set<StateID> ids;
    std::vector<QAction> opt_aops;
    std::vector<Distribution<StateID>> opt_transitions;

public:
    explicit ValueGraph(HeuristicSearchEngine<State, QAction, Interval>* hs)
        : base_engine_(hs)
    {
    }

    bool operator()(StateID qstate, std::vector<StateID>& successors)
    {
        assert(successors.empty());

        bool value_changed =
            base_engine_->compute_value_update_and_optimal_transitions(
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
};

template <typename State, typename Action, bool Interval>
class PolicyGraph {
    using QAction = typename quotients::QuotientSystem<State, Action>::QAction;

    HeuristicSearchEngine<State, QAction, Interval>* base_engine_;
    Distribution<StateID> t_;

public:
    explicit PolicyGraph(HeuristicSearchEngine<State, QAction, Interval>* hs)
        : base_engine_(hs)
    {
    }

    bool operator()(StateID qstate, std::vector<StateID>& successors)
    {
        t_.clear();
        bool result = base_engine_->apply_policy(qstate, t_);
        for (StateID sid : t_.elements()) {
            successors.push_back(sid);
        }
        return result;
    }
};

} // namespace internal

template <typename State, typename Action, bool Interval>
using FRETV = internal::FRET<
    State,
    Action,
    Interval,
    typename internal::ValueGraph<State, Action, Interval>>;

template <typename State, typename Action, bool Interval>
using FRETPi = internal::FRET<
    State,
    Action,
    Interval,
    typename internal::PolicyGraph<State, Action, Interval>>;

} // namespace fret
} // namespace engines
} // namespace probfd

#endif // __FRET_H__