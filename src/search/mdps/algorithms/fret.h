#pragma once

#include "../interfaces/i_engine.h"
#include "../interfaces/i_printable.h"
#include "../progress_report.h"
#include "heuristic_search_base.h"
#include "quotient_system.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"

#if defined(EXPENSIVE_STATISTICS)
#include "../../utils/timer.h"
#endif

#include <type_traits>

#if !defined(NDEBUG)
#define FRET_DEBUG_MSG(x) // x
#else
#define FRET_DEBUG_MSG(x)
#endif

namespace probabilistic {
namespace algorithms {
namespace fret {

namespace internal {

template<typename State, typename Action, typename B2>
using HeuristicSearch = heuristic_search_base::
    HeuristicSearchBase<State, Action, B2, std::true_type>;

struct Statistics : public IPrintable {
    unsigned long long iterations;
    unsigned long long traps;
    unsigned long long traps_dead;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer heuristic_search;
    utils::Timer trap_identification;
    utils::Timer trap_removal;
#endif

    Statistics()
        : iterations(0)
        , traps(0)
        , traps_dead(0)
    {
#if defined(EXPENSIVE_STATISTICS)
        heuristic_search.stop();
        trap_identification.stop();
        trap_removal.stop();
#endif
    }

    virtual void print(std::ostream& out) const override
    {
        out << "**** FRET ****" << std::endl;
        out << "Iterations: " << iterations << std::endl;
        out << "Permanent traps: " << traps << " (" << traps_dead << ")"
            << std::endl;
#if defined(EXPENSIVE_STATISTICS)
        out << "Heuristic search: " << heuristic_search << std::endl;
        out << "Trap identification: "
            << (trap_identification() - trap_removal()) << std::endl;
        out << "Trap removal: " << trap_removal << std::endl;
#endif
    }

    void register_report(ProgressReport* report)
    {
        report->register_print([this](std::ostream& out) {
            out << "fret=" << iterations << ", traps=" << traps;
        });
    }
};

struct TarjanStateInformation {
    constexpr static const unsigned UNDEF = -1;

    TarjanStateInformation()
        : onstack(false)
        , index(UNDEF)
        , lowlink(UNDEF)
    {
    }

    void open(const unsigned x)
    {
        onstack = true;
        index = x;
        lowlink = x;
    }

    bool onstack;
    unsigned index;
    unsigned lowlink;
};

template<
    typename State,
    typename Action,
    typename B2,
    typename GreedyGraphGenerator>
class FRET : public IMDPEngine<State> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<State, Action>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;

    FRET(
        GreedyGraphGenerator&& greedy_graph,
        ProgressReport* report,
        QuotientSystem* quotient,
        HeuristicSearch<QState, QAction, B2>* engine,
        StateListener<QState, QAction>* dead_end_listener)
        : greedy_graph_(std::move(greedy_graph))
        , report_(report)
        , quotient_(quotient)
        , base_engine_(engine)
        , listener_(dead_end_listener)
    {
    }

    virtual AnalysisResult solve(const State& state) override
    {
        statistics_.register_report(this->report_);
        AnalysisResult result;
        QState s(state);
        do {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.heuristic_search.resume();
#endif
            result = base_engine_->solve(s);
#if defined(EXPENSIVE_STATISTICS)
            statistics_.heuristic_search.stop();
            statistics_.trap_identification.resume();
#endif
            FRET_DEBUG_MSG(std::cout << "................. find&remove traps"
                                     << std::endl;)
            find_and_remove_traps(s);

#if defined(EXPENSIVE_STATISTICS)
            statistics_.trap_identification.stop();
#endif
            FRET_DEBUG_MSG(
                std::cout << "................. -> traps = " << trap_counter_
                          << " unexpanded = " << unexpanded_ << std::endl;)
            statistics_.iterations++;
            if (trap_counter_ + unexpanded_ == 0) {
                break;
            }
            base_engine_->reset_solver_state();
        } while (true);
        result.statistics =
            new IPrintables({ result.statistics, new Statistics(statistics_) });
        return result;
    }

private:
    struct ExplorationInfo {
        ExplorationInfo(const StateID& state_id, const QState& qstate)
            : state_id(state_id)
            , state(qstate)
            , is_leaf(true)
        {
        }
        StateID state_id;
        QState state;
        std::vector<QState> successors;
        bool is_leaf;
    };

    void find_and_remove_traps(const QState& state)
    {
        trap_counter_ = 0;
        unexpanded_ = 0;

        storage::StateHashMap<TarjanStateInformation> state_infos;
        std::deque<ExplorationInfo> exploration_queue;
        std::deque<StateID> stack;
        unsigned current_index = 0;

        {
            StateID stateid = quotient_->get_representative_id(state);
            push(stateid, state, exploration_queue);
            stack.push_back(stateid);
            state_infos[stateid].open(current_index);
        }

        bool backtracked = false;
        unsigned last_lowlink = TarjanStateInformation::UNDEF;
        while (!exploration_queue.empty()) {
            ExplorationInfo& einfo = exploration_queue.back();
            TarjanStateInformation& sinfo = state_infos[einfo.state_id];
            sinfo.lowlink = std::min(sinfo.lowlink, last_lowlink);
            einfo.is_leaf = einfo.is_leaf && !backtracked;
            bool fully_explored = true;
            while (!einfo.successors.empty()) {
                QState succ = einfo.successors.back();
                StateID succid = quotient_->get_representative_id(succ);
                einfo.successors.pop_back();
                TarjanStateInformation& succ_info = state_infos[succid];
                if (succ_info.index == TarjanStateInformation::UNDEF) {
                    if (push(succid, succ, exploration_queue)) {
                        succ_info.open(++current_index);
                        stack.push_front(succid);
                        backtracked = false;
                        last_lowlink = TarjanStateInformation::UNDEF;
                        fully_explored = false;
                        break;
                    } else {
                        einfo.is_leaf = false;
                    }
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
                    } while (*(it++) != einfo.state_id);
                    if (einfo.is_leaf) {
                        if (scc_size > 1) {
                            FRET_DEBUG_MSG(
                                std::cout << "=====> TRAP = [";
                                for (auto x = stack.begin(); x != it;
                                     ++x) { std::cout << " " << *x; } std::cout
                                << " ] ... " << std::flush;)
                            trap_counter_++;
#if defined(EXPENSIVE_STATISTICS)
                            statistics_.trap_removal.resume();
#endif
                            quotient_->build_quotient(
                                stack.begin(), it, einfo.state_id);
#if defined(EXPENSIVE_STATISTICS)
                            statistics_.trap_removal.stop();
#endif
                            FRET_DEBUG_MSG(std::cout << " ... -> "
                                                     << einfo.state_id
                                                     << std::endl;)
                            QAction policy = NullAction<QAction>()();
                            base_engine_->async_update(
                                einfo.state, &policy, nullptr);
                            if (listener_ != nullptr
                                && policy == NullAction<QAction>()()) {
                                base_engine_->set_dead_end(einfo.state_id);
                                listener_->operator()(einfo.state);
                            }
                            statistics_.traps++;
                        } else if (
                            listener_ != nullptr
                            && base_engine_->conditional_set_dead_end(
                                einfo.state_id)) {
                            listener_->operator()(einfo.state);
                        }
                    }
                    stack.erase(stack.begin(), it);
                }
                exploration_queue.pop_back();
            }
        }
    }

    bool push(
        const StateID& state_id,
        const State& state,
        std::deque<ExplorationInfo>& queue)
    {
        if (base_engine_->is_terminal(state_id)) {
            return false;
        }
        queue.emplace_back(state_id, state);
        if (!greedy_graph_(state, queue.back().successors)) {
            unexpanded_++;
            queue.pop_back();
            return false;
        }
        return true;
    }

    GreedyGraphGenerator greedy_graph_;
    ProgressReport* report_;
    QuotientSystem* quotient_;
    HeuristicSearch<QState, QAction, B2>* base_engine_;
    StateListener<QState, QAction>* listener_;

    Statistics statistics_;
    bool last_dead_;
    Distribution<QState> successors_;
    unsigned trap_counter_;
    unsigned unexpanded_;
};

template<typename State, typename Action, typename B2>
class ValueGraph {
public:
    using QuotientSystem = quotient_system::QuotientSystem<State, Action>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;

    explicit ValueGraph(
        QuotientSystem* q,
        HeuristicSearch<QState, QAction, B2>* hs)
        : base_engine_(hs)
        , collector_(q)
    {
    }

    bool operator()(const QState& qstate, std::vector<QState>& successors)
    {
        collector_.states.clear();
        if (base_engine_->async_update(
                qstate, &collector_, nullptr, nullptr, nullptr)
            || collector_.states.empty()) {
            return false;
        }
        collector_.states.swap(successors);
        return true;
    }

private:
    class StateCollector {
    public:
        explicit StateCollector(QuotientSystem* q)
            : q_(q)
        {
        }
        int operator()(
            const bool,
            const QAction*,
            const QState&,
            const std::vector<std::pair<QAction, Distribution<QState>>>&
                transisions)
        {
            for (int i = transisions.size() - 1; i >= 0; --i) {
                const Distribution<QState>& t = transisions[i].second;
                for (auto it = t.begin(); it != t.end(); ++it) {
                    if (ids.insert(q_->get_id(it->first)).second) {
                        states.push_back(it->first);
                    }
                }
            }
            ids.clear();
            return -1;
        }
        std::vector<QState> states;

    private:
        std::unordered_set<StateID> ids;
        QuotientSystem* q_;
    };

    HeuristicSearch<QState, QAction, B2>* base_engine_;
    StateCollector collector_;
};

template<typename State, typename Action, typename B2>
class PolicyGraph {
public:
    using QuotientSystem = quotient_system::QuotientSystem<State, Action>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;

    explicit PolicyGraph(HeuristicSearch<QState, QAction, B2>* hs)
        : base_engine_(hs)
    {
    }

    bool operator()(const QState& qstate, std::vector<QState>& successors)
    {
        t_.clear();
        base_engine_->apply_policy(qstate, t_);
        if (t_.empty()) {
            return false;
        }
        for (auto it = t_.begin(); it != t_.end(); ++it) {
            successors.push_back(it->first);
        }
        return true;
    }

private:
    HeuristicSearch<QState, QAction, B2>* base_engine_;
    Distribution<QState> t_;
};

} // namespace internal

template<typename State, typename Action, typename B2>
class FRETV : public internal::FRET<
                  State,
                  Action,
                  B2,
                  typename internal::ValueGraph<State, Action, B2>> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<State, Action>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;

    FRETV(
        ProgressReport* report,
        QuotientSystem* quotient,
        internal::HeuristicSearch<QState, QAction, B2>* engine,
        StateListener<QState, QAction>* dead_end_listener)
        : internal::FRET<
            State,
            Action,
            B2,
            typename internal::ValueGraph<State, Action, B2>>(
            typename internal::ValueGraph<State, Action, B2>(quotient, engine),
            report,
            quotient,
            engine,
            dead_end_listener)
    {
    }
};

template<typename State, typename Action, typename B2>
class FRETPi : public internal::FRET<
                   State,
                   Action,
                   B2,
                   typename internal::PolicyGraph<State, Action, B2>> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<State, Action>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;

    FRETPi(
        ProgressReport* report,
        QuotientSystem* quotient,
        internal::HeuristicSearch<QState, QAction, B2>* engine,
        StateListener<QState, QAction>* dead_end_listener)
        : internal::FRET<
            State,
            Action,
            B2,
            typename internal::PolicyGraph<State, Action, B2>>(
            typename internal::PolicyGraph<State, Action, B2>(engine),
            report,
            quotient,
            engine,
            dead_end_listener)
    {
    }
};

} // namespace fret
} // namespace algorithms
} // namespace probabilistic

#undef FRET_DEBUG_MSG
