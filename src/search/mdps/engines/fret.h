#ifndef MDPS_ENGINES_FRET_H
#define MDPS_ENGINES_FRET_H

#include "../progress_report.h"
#include "../quotient_system/quotient_system.h"
#include "../utils/graph_visualization.h"
#include "engine.h"
#include "heuristic_search_base.h"

#if defined(EXPENSIVE_STATISTICS)
#include "../../utils/timer.h"
#endif

#include <fstream>
#include <sstream>
#include <type_traits>

#if !defined(NDEBUG)
#define FRET_DEBUG_MSG(x) // x
#else
#define FRET_DEBUG_MSG(x)
#endif

namespace probabilistic {
namespace engines {

/// Namespace dedicated to the Find, Revise, Eliminate Traps (FRET) framework.
namespace fret {

template <typename State, typename Action, typename B2>
using HeuristicSearchEngine =
    heuristic_search::HeuristicSearchBase<State, Action, B2, std::true_type>;

namespace internal {

struct Statistics {
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

    void register_report(ProgressReport* report)
    {
        report->register_print([this](std::ostream& out) {
            out << "fret=" << iterations << ", traps=" << traps;
        });
    }
};

struct TarjanStateInformation {
    constexpr static const unsigned UNDEF = -1;

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

template <
    typename State,
    typename Action,
    typename B2,
    typename GreedyGraphGenerator>
class FRET : public MDPEngine<State, Action> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using QAction = typename QuotientSystem::QAction;

    FRET(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        GreedyGraphGenerator&& greedy_graph,
        QuotientSystem* quotient,
        ProgressReport* report,
        HeuristicSearchEngine<State, QAction, B2>* engine)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              state_reward_function,
              action_reward_function,
              minimal_reward,
              maximal_reward,
              aops_generator,
              transition_generator)
        , greedy_graph_(std::move(greedy_graph))
        , report_(report)
        , quotient_(quotient)
        , base_engine_(engine)
    {
    }

    virtual void solve(const State& state) override
    {
        statistics_.register_report(this->report_);
        do {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.heuristic_search.resume();
#endif
            base_engine_->solve(state);
#if defined(EXPENSIVE_STATISTICS)
            statistics_.heuristic_search.stop();
            statistics_.trap_identification.resume();
#endif
            FRET_DEBUG_MSG(std::cout << "................. find&remove traps"
                                     << std::endl;)
            find_and_remove_traps(state);

#if defined(EXPENSIVE_STATISTICS)
            statistics_.trap_identification.stop();
#endif
            FRET_DEBUG_MSG(
                std::cout << "................. -> traps = " << trap_counter_
                          << " unexpanded = " << unexpanded_ << std::endl;)
            statistics_.iterations++;
            // std::cout << "fret#" << statistics_.iterations << " -> traps=" <<
            // trap_counter_ << ", unexpanded=" << unexpanded_ << std::endl;
            // dump_quotient_system(s);
            if (trap_counter_ + unexpanded_ == 0) {
                break;
            }
            base_engine_->reset_solver_state();
        } while (true);
    }

    virtual value_type::value_t get_result(const State& state) override
    {
        return base_engine_->get_result(state);
    }

    virtual bool supports_error_bound() const override
    {
        return base_engine_->supports_error_bound();
    }

    virtual value_type::value_t get_error(const State& state) override
    {
        return base_engine_->get_error(state);
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        this->base_engine_->print_statistics(out);
        statistics_.print(out);
    }

#if 0
    void dump_quotient_system(const State& initial_state) const
    {
        std::ofstream out;
        {
            std::ostringstream filename;
            filename << "fret_qs_" << statistics_.iterations << ".dot";
            out.open(filename.str());
        }
        auto idmap = quotient_->create_state_id_map();
        auto aops = quotient_->create_aops_generator();
        auto tgen = quotient_->create_transition_generator();
        auto term = [this](const State& s) {
            StateID sid = quotient_->get_representative_id(s);
            return base_engine_->is_terminal(sid);
        };
        auto prune = [this](const State& s) {
            StateID sid = quotient_->get_representative_id(s);
            const QAction& a = *base_engine_->get_policy(sid);
            return a == NullAction<QAction>()();
        };
        auto print_a = [this](const QAction& a) {
            StateID sid = quotient_->get_representative_id(a.first);
            const QAction& b = *base_engine_->get_policy(sid);
            return a == b ? "*" : "";
        };
        auto print_s = [this](const State& s) {
            StateID sid = quotient_->get_representative_id(s);
            std::ostringstream res;
            res << "[";
            auto it = quotient_->quotient_iterator(sid);
            bool first = true;
            while (it.first != it.second) {
                res << (first ? "" : ", ") << *it.first;
                first = false;
                it.first++;
            }
            res << "] (" << base_engine_->get_value(sid) << ")";
            return res.str();
        };
        graphviz::GraphVisualization<
            State,
            QAction,
            decltype(print_s),
            decltype(print_a),
            decltype(term),
            decltype(prune)>
            gv(idmap, aops, tgen, &term, &print_s, &print_a, false, &prune);
        gv(out, initial_state);
        delete (idmap);
        delete (aops);
        delete (tgen);
        out.close();
    }
#endif

private:
    struct ExplorationInfo {
        ExplorationInfo(const StateID& state_id)
            : state_id(state_id)
            , is_leaf(true)
        {
        }
        StateID state_id;
        std::vector<StateID> successors;
        bool is_leaf;
    };

    void find_and_remove_traps(const State& state)
    {
        trap_counter_ = 0;
        unexpanded_ = 0;

        storage::StateHashMap<TarjanStateInformation> state_infos;
        std::deque<ExplorationInfo> exploration_queue;
        std::deque<StateID> stack;
        unsigned current_index = 0;

        {
            StateID stateid = this->get_state_id(state);
            push(stateid, exploration_queue);
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
                StateID succid = einfo.successors.back();
                einfo.successors.pop_back();
                TarjanStateInformation& succ_info = state_infos[succid];
                if (succ_info.index == TarjanStateInformation::UNDEF) {
                    if (push(succid, exploration_queue)) {
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
                                stack.begin(),
                                it,
                                einfo.state_id);
                            base_engine_->clear_policy(einfo.state_id);
#if defined(EXPENSIVE_STATISTICS)
                            statistics_.trap_removal.stop();
#endif
                            FRET_DEBUG_MSG(std::cout << " ... -> "
                                                     << einfo.state_id
                                                     << std::endl;)
                            base_engine_->async_update(einfo.state_id);
                            if (base_engine_->is_marked_dead_end(
                                    einfo.state_id)) {
                                base_engine_->notify_dead_end(einfo.state_id);
                            }
                            statistics_.traps++;
                        } else {
                            base_engine_->conditional_notify_dead_end(
                                einfo.state_id);
                        }
                    }
                    stack.erase(stack.begin(), it);
                }
                exploration_queue.pop_back();
            }
        }
    }

    bool push(const StateID& state_id, std::deque<ExplorationInfo>& queue)
    {
        if (base_engine_->is_terminal(state_id)) {
            return false;
        }
        std::vector<StateID> succs;
        bool sth = greedy_graph_(state_id, succs);
        if (sth) {
            ++unexpanded_;
        }
        if (succs.empty()) {
            return false;
        }
        queue.emplace_back(state_id);
        queue.back().successors.swap(succs);
        return true;
    }

    GreedyGraphGenerator greedy_graph_;
    ProgressReport* report_;
    QuotientSystem* quotient_;
    HeuristicSearchEngine<State, QAction, B2>* base_engine_;

    Statistics statistics_;
    bool last_dead_;
    unsigned trap_counter_;
    unsigned unexpanded_;
};

template <typename State, typename Action, typename B2>
class ValueGraph {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using QAction = typename QuotientSystem::QAction;

    explicit ValueGraph(HeuristicSearchEngine<State, QAction, B2>* hs)
        : base_engine_(hs)
        , collector_()
    {
    }

    bool operator()(const StateID& qstate, std::vector<StateID>& successors)
    {
        collector_.states.clear();
        bool result =
            base_engine_
                ->async_update(qstate, &collector_, nullptr, nullptr, nullptr);
        collector_.states.swap(successors);
        return result;
    }

private:
    class StateCollector {
    public:
        int operator()(
            const StateID,
            const ActionID,
            const std::vector<QAction>&,
            const std::vector<Distribution<StateID>>& transisions)
        {
            for (int i = transisions.size() - 1; i >= 0; --i) {
                const Distribution<StateID>& t = transisions[i];
                for (auto it = t.begin(); it != t.end(); ++it) {
                    if (ids.insert(it->first).second) {
                        states.push_back(it->first);
                    }
                }
            }
            ids.clear();
            return -1;
        }

        std::vector<StateID> states;

    private:
        std::unordered_set<StateID> ids;
    };

    HeuristicSearchEngine<State, QAction, B2>* base_engine_;
    StateCollector collector_;
};

template <typename State, typename Action, typename B2>
class PolicyGraph {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using QAction = typename QuotientSystem::QAction;

    explicit PolicyGraph(HeuristicSearchEngine<State, QAction, B2>* hs)
        : base_engine_(hs)
    {
    }

    bool operator()(const StateID& qstate, std::vector<StateID>& successors)
    {
        t_.clear();
        bool result = base_engine_->apply_policy(qstate, t_);
        for (auto it = t_.begin(); it != t_.end(); ++it) {
            successors.push_back(it->first);
        }
        return result;
    }

private:
    HeuristicSearchEngine<State, QAction, B2>* base_engine_;
    Distribution<StateID> t_;
};

} // namespace internal

template <typename State, typename Action, typename B2>
class FRETV
    : public internal::FRET<
          State,
          Action,
          B2,
          typename internal::ValueGraph<State, Action, B2>> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using QAction = typename QuotientSystem::QAction;

    using ValueGraph = typename internal::ValueGraph<State, Action, B2>;

    template <typename... Args>
    FRETV(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        QuotientSystem* quotient,
        ProgressReport* report,
        HeuristicSearchEngine<State, QAction, B2>* engine)
        : internal::FRET<State, Action, B2, ValueGraph>(
              state_id_map,
              action_id_map,
              state_reward_function,
              action_reward_function,
              minimal_reward,
              maximal_reward,
              aops_generator,
              transition_generator,
              ValueGraph(engine),
              quotient,
              report,
              engine)
    {
    }
};

template <typename State, typename Action, typename B2>
class FRETPi
    : public internal::FRET<
          State,
          Action,
          B2,
          typename internal::PolicyGraph<State, Action, B2>> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using QAction = typename QuotientSystem::QAction;

    using PolicyGraph = typename internal::PolicyGraph<State, Action, B2>;

    FRETPi(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        QuotientSystem* quotient,
        ProgressReport* report,
        HeuristicSearchEngine<State, QAction, B2>* engine)
        : internal::FRET<State, Action, B2, PolicyGraph>(
              state_id_map,
              action_id_map,
              state_reward_function,
              action_reward_function,
              minimal_reward,
              maximal_reward,
              aops_generator,
              transition_generator,
              PolicyGraph(engine),
              quotient,
              report,
              engine)
    {
    }
};

} // namespace fret
} // namespace engines
} // namespace probabilistic

#undef FRET_DEBUG_MSG

#endif // __FRET_H__