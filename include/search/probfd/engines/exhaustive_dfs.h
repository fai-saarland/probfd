#ifndef PROBFD_ENGINES_EXHAUSTIVE_DFS_H
#define PROBFD_ENGINES_EXHAUSTIVE_DFS_H

#include "probfd/engines/heuristic_search_state_information.h"
#include "probfd/engines/utils.h"

#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/state_properties.h"
#include "probfd/engine_interfaces/transition_sorter.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/engine.h"
#include "probfd/progress_report.h"

#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <deque>
#include <limits>
#include <memory>

namespace probfd {
namespace engines {

/// namespace for anytime TVI
namespace exhaustive_dfs {

struct Statistics {
    unsigned long long expanded = 0;
    unsigned long long evaluations = 0;
    unsigned long long evaluated = 0;
    unsigned long long goal_states = 0;
    unsigned long long transitions = 0;
    unsigned long long dead_ends = 0;
    unsigned long long terminal = 0;
    unsigned long long self_loop = 0;

    unsigned long long value_updates = 0;
    unsigned long long backtracks = 0;
    unsigned long long sccs = 0;
    unsigned long long dead_end_sccs = 0;
    unsigned long long pruned_dead_end_sccs = 0;
    unsigned long long summed_dead_end_scc_sizes = 0;

    void print(std::ostream& out) const
    {
        out << "  Expanded " << expanded << " state(s)." << std::endl;
        out << "  Evaluated " << evaluated << " state(s)." << std::endl;
        out << "  Evaluations: " << evaluations << std::endl;
        out << "  Terminal states: " << terminal << std::endl;
        out << "  Pure self-loop states: " << self_loop << std::endl;
        out << "  Goal states: " << goal_states << " state(s)." << std::endl;
        out << "  Dead ends: " << dead_ends << " state(s)." << std::endl;
        out << "  State value updates: " << value_updates << std::endl;
        out << "  Backtracked from " << backtracks << " state(s)." << std::endl;
        out << "  Found " << sccs << " SCC(s)." << std::endl;
        out << "  Found " << dead_end_sccs << " dead-end SCC(s)." << std::endl;
        out << "  Partially pruned " << pruned_dead_end_sccs
            << " dead-end SCC(s)." << std::endl;
        out << "  Average dead-end SCC size: "
            << (static_cast<double>(summed_dead_end_scc_sizes) /
                static_cast<int>(dead_end_sccs))
            << std::endl;
    }
};

inline bool update_lower_bound(value_t& x, value_t v)
{
    if (v > x) {
        x = v;
        return true;
    }

    return false;
}

inline bool update_lower_bound(Interval& x, value_t v)
{
    if (v > x.lower) {
        x.lower = v;
        return true;
    }

    return false;
}

/**
 * @brief Implementation of an anytime topological value iteration
 * variant.
 *
 * \todo Add some implementation notes.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 *
 */
template <typename State, typename Action, bool UseInterval>
class ExhaustiveDepthFirstSearch : public MDPEngine<State, Action> {
    using Base = typename ExhaustiveDepthFirstSearch::MDPEngine;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using NewStateObserver = engine_interfaces::NewStateObserver<State>;
    using TransitionSorter = engine_interfaces::TransitionSorter<Action>;

    using EngineValueType = engines::EngineValueType<UseInterval>;

    struct SearchNodeInformation {
        static constexpr uint8_t NEW = 0;
        static constexpr uint8_t CLOSED = 1;
        static constexpr uint8_t OPEN = 2;
        static constexpr uint8_t ONSTACK = 4;
        static constexpr uint8_t DEAD = 3;
        static constexpr uint8_t MARKED = 7;

        // TODO store lowlink in hash map -> only required for states still on
        // stack
        unsigned lowlink = -1;
        uint8_t status = NEW;
        EngineValueType value;
        value_t state_cost;

        bool is_new() const { return status == NEW; }
        bool is_open() const { return status == OPEN; }
        bool is_onstack() const { return status == ONSTACK; }
        bool is_closed() const { return status & CLOSED; }
        bool is_dead_end() const { return status == DEAD || status == MARKED; }
        bool is_marked_dead_end() const { return status == MARKED; }

        void open()
        {
            assert(is_new());
            status = OPEN;
        }

        void set_onstack(const unsigned idx)
        {
            assert(is_open());
            status = ONSTACK;
            lowlink = idx;
        }

        void close() { status = CLOSED; }

        void set_dead_end()
        {
            assert(!is_marked_dead_end());
            status = DEAD;
        }

        void mark_dead_end() { status = MARKED; }

        value_t get_value() const
        {
            if constexpr (UseInterval) {
                return value.lower;
            } else {
                return value;
            }
        }

        Interval get_bounds() const
        {
            if constexpr (!UseInterval) {
                return Interval(value, INFINITE_VALUE);
            } else {
                return value;
            }
        }
    };

    struct ExpansionInformation {
        explicit ExpansionInformation(
            unsigned stack_index,
            unsigned neighbors_size)
            : stack_index(stack_index)
            , neighbors_size(neighbors_size)
        {
        }

        std::vector<Distribution<StateID>> successors;
        Distribution<StateID>::const_iterator succ;
        unsigned stack_index;
        unsigned neighbors_size;

        bool all_successors_are_dead = true;
        bool all_successors_marked_dead = true;

        void update_successors_dead(bool successors_dead)
        {
            all_successors_are_dead =
                all_successors_are_dead && successors_dead;
        }
    };

    struct SCCTransition {
        Distribution<StateID> successors;
        value_t base = 0_vt;
        value_t self_loop = 0_vt;
    };

    struct StackInformation {
        explicit StackInformation(StateID state_ref)
            : state_ref(state_ref)
        {
        }

        std::vector<SCCTransition> successors;
        StateID state_ref;
        int i = -1;
    };

    struct SearchNodeInfos : public engine_interfaces::StateProperties {
        storage::PerStateStorage<SearchNodeInformation> infos;

        SearchNodeInformation& operator[](StateID state_id)
        {
            return infos[state_id];
        }

        const SearchNodeInformation& operator[](StateID state_id) const
        {
            return infos[state_id];
        }

        engines::heuristic_search::StateFlags&
        lookup_state_flags(StateID) override
        {
            ABORT("Exhaustive DFS does not store state flags.");
        }

        value_t lookup_value(StateID state_id) override
        {
            return infos[state_id].get_value();
        }

        Interval lookup_bounds(StateID state_id) override
        {
            return infos[state_id].get_bounds();
        }
    };

    std::shared_ptr<NewStateObserver> new_state_handler_;
    std::shared_ptr<TransitionSorter> transition_sort_;

    ProgressReport* report_;
    const Interval cost_bound_;
    const EngineValueType trivial_bound_;

    const bool value_propagation_;
    const bool only_propagate_when_changed_;
    const bool evaluator_recomputation_;
    const bool notify_initial_state_;

    Statistics statistics_;
    SearchNodeInfos search_space_;

    std::deque<ExpansionInformation> expansion_infos_;
    std::deque<StackInformation> stack_infos_;
    std::vector<StateID> neighbors_;

    bool last_all_dead_ = true;
    bool last_all_marked_dead_ = true;
    bool backtracked_from_dead_end_scc_ = false;

public:
    explicit ExhaustiveDepthFirstSearch(
        std::shared_ptr<NewStateObserver> new_state_handler,
        std::shared_ptr<TransitionSorter> transition_sorting,
        Interval cost_bound,
        bool reevaluate,
        bool notify_initial,
        bool path_updates,
        bool only_propagate_when_changed,
        ProgressReport* progress)
        : new_state_handler_(new_state_handler)
        , transition_sort_(transition_sorting)
        , report_(progress)
        , cost_bound_(cost_bound)
        , trivial_bound_(get_trivial_bound())
        , value_propagation_(path_updates)
        , only_propagate_when_changed_(only_propagate_when_changed)
        , evaluator_recomputation_(reevaluate)
        , notify_initial_state_(notify_initial)
    {
    }

    Interval
    solve(MDP& mdp, Evaluator& heuristic, param_type<State> state, double)
        override
    {
        StateID stateid = mdp.get_state_id(state);
        SearchNodeInformation& info = search_space_[stateid];
        if (!initialize_search_node(mdp, heuristic, state, info)) {
            return search_space_.lookup_bounds(stateid);
        }

        if (!push_state(mdp, heuristic, stateid, info)) {
            std::cout << "initial state is dead end!" << std::endl;
            return search_space_.lookup_bounds(stateid);
        }

        register_value_reports(&info);
        run_exploration(mdp, heuristic);

        return search_space_.lookup_bounds(stateid);
    }

    void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    void register_value_reports(const SearchNodeInformation* info)
    {
        if constexpr (UseInterval) {
            this->report_->register_bound("v", [info]() {
                return info->value;
            });
        } else {
            this->report_->register_bound("v", [info]() {
                return Interval(info->value, INFINITE_VALUE);
            });
        }
    }

    EngineValueType get_trivial_bound() const
    {
        if constexpr (UseInterval) {
            return cost_bound_;
        } else {
            return cost_bound_.upper;
        }
    }

    bool initialize_search_node(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state_id,
        SearchNodeInformation& info)
    {
        return initialize_search_node(
            mdp,
            heuristic,
            mdp.get_state(state_id),
            info);
    }

    bool initialize_search_node(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        SearchNodeInformation& info)
    {
        assert(info.is_new());
        info.value = trivial_bound_;

        TerminationInfo term_info = mdp.get_termination_info(state);
        info.state_cost = term_info.get_cost();
        if (term_info.is_goal_state()) {
            info.close();
            info.value = EngineValueType(term_info.get_cost());
            ++statistics_.goal_states;
            if (new_state_handler_) {
                new_state_handler_->notify_goal(state);
            }
            return false;
        }

        EvaluationResult eval_result = heuristic.evaluate(state);
        if (eval_result.is_unsolvable()) {
            info.value = EngineValueType(info.state_cost);
            info.mark_dead_end();
            ++statistics_.dead_ends;
            if (new_state_handler_) {
                new_state_handler_->notify_dead(state);
            }
            return false;
        }

        if constexpr (UseInterval) {
            info.value.upper = eval_result.get_estimate();
        }

        info.open();
        if (new_state_handler_) {
            new_state_handler_->notify_state(state);
        }

        return true;
    }

    bool push_state(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state_id,
        SearchNodeInformation& info)
    {
        std::vector<Action> aops;
        std::vector<Distribution<StateID>> successors;
        mdp.generate_all_transitions(state_id, aops, successors);
        if (successors.empty()) {
            info.value = EngineValueType(info.state_cost);
            info.set_dead_end();
            statistics_.terminal++;
            return false;
        }

        statistics_.expanded++;

        if (transition_sort_ != nullptr) {
            transition_sort_->sort(state_id, aops, successors, search_space_);
        }

        expansion_infos_.emplace_back(stack_infos_.size(), neighbors_.size());
        stack_infos_.emplace_back(state_id);

        ExpansionInformation& exp = expansion_infos_.back();
        StackInformation& si = stack_infos_.back();

        si.successors.resize(aops.size());

        const auto cost = info.get_value();

        bool pure_self_loop = true;

        unsigned j = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            auto& succs = successors[i];
            auto& t = si.successors[i];
            bool all_self_loops = true;

            succs.remove_if([&, this, state_id](auto& elem) {
                const auto [succ_id, prob] = elem;

                // Remove self loops
                if (succ_id == state_id) {
                    t.self_loop += prob;
                    return true;
                }

                SearchNodeInformation& succ_info = search_space_[succ_id];
                if (succ_info.is_new()) {
                    initialize_search_node(mdp, heuristic, succ_id, succ_info);
                }

                if (succ_info.is_closed()) {
                    t.base += prob * succ_info.get_value();
                    exp.update_successors_dead(succ_info.is_dead_end());
                    exp.all_successors_marked_dead =
                        exp.all_successors_marked_dead &&
                        succ_info.is_marked_dead_end();

                    all_self_loops = false;

                    return true;
                }

                return false;
            });

            const auto& a = aops[i];
            if (succs.empty()) {
                if (!all_self_loops) {
                    pure_self_loop = false;
                    t.base += cost + mdp.get_action_cost(a);
                    auto non_loop = 1_vt - t.self_loop;
                    update_lower_bound(info.value, t.base / non_loop);
                }
            } else {
                t.base += cost + mdp.get_action_cost(a);

                if (t.self_loop == 0_vt) {
                    t.self_loop = 1_vt;
                } else {
                    assert(t.self_loop < 1_vt);
                    t.self_loop = 1_vt / (1_vt - t.self_loop);
                }

                if (i != j) {
                    si.successors[j] = std::move(si.successors[i]);
                    successors[j] = std::move(successors[i]);
                }
                ++j;
            }
        }

        if (j == 0) {
            expansion_infos_.pop_back();
            stack_infos_.pop_back();

            if (pure_self_loop) {
                info.value = EngineValueType(info.state_cost);
                info.set_dead_end();
                ++statistics_.self_loop;
            } else {
                info.value = EngineValueType(info.get_value());
                info.close();
            }

            return false;
        }

        successors.erase(successors.begin() + j, successors.end());
        si.successors.erase(si.successors.begin() + j, si.successors.end());
        si.i = 0;

        info.set_onstack(stack_infos_.size() - 1);
        exp.successors = std::move(successors);
        exp.succ = exp.successors.back().begin();

        return true;
    }

    void run_exploration(MDP& mdp, Evaluator& heuristic)
    {
        while (!expansion_infos_.empty()) {
            ExpansionInformation& expanding = expansion_infos_.back();
            assert(expanding.stack_index < stack_infos_.size());
            assert(!expanding.successors.empty());
            assert(expanding.succ != expanding.successors.back().end());

            StackInformation& stack_info = stack_infos_[expanding.stack_index];
            assert(!stack_info.successors.empty());

            const StateID stateid = stack_info.state_ref;
            SearchNodeInformation& node_info = search_space_[stateid];

            expanding.update_successors_dead(last_all_dead_);
            expanding.all_successors_marked_dead =
                expanding.all_successors_marked_dead && last_all_marked_dead_;

            int idx = stack_info.successors.size() - stack_info.i - 1;
            SCCTransition* inc = &stack_info.successors[idx];
            bool val_changed = false;
            bool completely_explored = false;

            for (;;) {
                for (; expanding.succ != expanding.successors.back().end();
                     ++expanding.succ) {
                    const auto [succ_id, prob] = *expanding.succ;

                    assert(succ_id != stateid);
                    SearchNodeInformation& succ_info = search_space_[succ_id];
                    assert(!succ_info.is_new());

                    if (succ_info.is_open()) {
                        if (push_state(mdp, heuristic, succ_id, succ_info)) {
                            goto skip;
                        }

                        expanding.update_successors_dead(
                            succ_info.is_dead_end());
                        expanding.all_successors_marked_dead =
                            expanding.all_successors_are_dead &&
                            succ_info.is_marked_dead_end();
                        inc->base += prob * succ_info.get_value();
                    } else if (succ_info.is_onstack()) {
                        node_info.lowlink =
                            std::min(node_info.lowlink, succ_info.lowlink);
                        inc->successors.add_probability(succ_id, prob);
                    } else {
                        assert(succ_info.is_closed());
                        expanding.update_successors_dead(
                            succ_info.is_dead_end());
                        expanding.all_successors_marked_dead =
                            expanding.all_successors_are_dead &&
                            succ_info.is_marked_dead_end();
                        inc->base += prob * succ_info.get_value();
                    }
                }

                expanding.successors.pop_back();
                if (update_lower_bound(
                        node_info.value,
                        inc->base * inc->self_loop)) {
                    val_changed = true;
                    if (check_early_convergence(node_info)) {
                        expanding.successors.clear();
                    }
                }

                if (expanding.successors.empty()) {
                    if (inc->successors.empty()) {
                        if (stack_info.i > 0)
                            std::swap(stack_info.successors.back(), *inc);
                        stack_info.successors.pop_back();
                    }

                    break;
                }

                if (inc->successors.empty()) {
                    if (stack_info.i > 0) {
                        std::swap(stack_info.successors.back(), *inc);
                    }

                    stack_info.successors.pop_back();
                    int t = stack_info.successors.size() - stack_info.i - 1;
                    inc = &stack_info.successors[t];
                } else {
                    --inc;
                    ++stack_info.i;
                }

                expanding.succ = expanding.successors.back().begin();
            }

            backtracked_from_dead_end_scc_ = false;
            last_all_dead_ = true;
            last_all_marked_dead_ = true;

            last_all_dead_ = expanding.all_successors_are_dead;
            last_all_marked_dead_ = expanding.all_successors_marked_dead;
            statistics_.backtracks++;

            if (expanding.stack_index == node_info.lowlink) {
                ++statistics_.sccs;

                auto rend = stack_infos_.rbegin();
                if (expanding.all_successors_are_dead) {
                    unsigned scc_size = 0;
                    do {
                        ++scc_size;
                        auto& info = search_space_[rend->state_ref];
                        info.value = EngineValueType(info.state_cost);
                        info.set_dead_end();
                    } while ((rend++)->state_ref != stateid);

                    statistics_.dead_end_sccs++;
                    statistics_.summed_dead_end_scc_sizes += scc_size;
                } else {
                    unsigned scc_size = 0;
                    do {
                        auto& info = search_space_[rend->state_ref];
                        info.close();

                        if constexpr (UseInterval) {
                            val_changed =
                                update(
                                    info.value,
                                    EngineValueType(info.value.lower)) ||
                                val_changed;
                        }

                        ++scc_size;
                    } while ((rend++)->state_ref != stateid);

                    if (scc_size > 1) {
                        unsigned iterations = 0;
                        bool changed = true;
                        do {
                            changed = false;
                            for (auto it = stack_infos_.rbegin(); it != rend;
                                 ++it) {
                                StackInformation& s = *it;
                                assert(!s.successors.empty());
                                value_t best = s.successors.back().base;
                                for (int i = s.successors.size() - 1; i >= 0;
                                     --i) {
                                    const auto& t = s.successors[i];
                                    value_t t_first = t.base;
                                    for (auto [succ_id, prob] : t.successors) {
                                        t_first +=
                                            prob *
                                            search_space_[succ_id].get_value();
                                    }
                                    t_first = t_first * t.self_loop;
                                    best = best > t_first ? best : t_first;
                                }

                                SearchNodeInformation& snode_info =
                                    search_space_[s.state_ref];
                                if (best > snode_info.get_value()) {
                                    changed =
                                        changed || !is_approx_equal(
                                                       snode_info.get_value(),
                                                       best);
                                    snode_info.value = EngineValueType(best);
                                }
                            }
                            ++iterations;
                        } while (changed);

                        val_changed = val_changed || iterations > 1;
                    }
                }

                stack_infos_.erase(rend.base(), stack_infos_.end());
            }

            expansion_infos_.pop_back();

            completely_explored = true;

        skip:

            if ((val_changed || !only_propagate_when_changed_) 
                    && value_propagation_) {
                propagate_value_along_trace(
                    completely_explored,
                    node_info.get_value());
            }
        }
    }

    void propagate_value_along_trace(bool was_poped, value_t val)
    {
        auto it = expansion_infos_.rbegin();
        if (!was_poped) {
            it += 2;
        }

        for (; it != expansion_infos_.rend(); ++it) {
            StackInformation& st = stack_infos_[it->stack_index];
            SearchNodeInformation& sn = search_space_[st.state_ref];
            const auto& t = st.successors[st.successors.size() - st.i - 1];
            const value_t v = t.base + it->succ->probability * val;
            if (!update_lower_bound(sn.value, v)) {
                break;
            }

            val = v;
        }

        if (it == expansion_infos_.rend()) {
            report_->print();
        }
    }

    void mark_current_component_dead()
    {
        statistics_.sccs++;
        statistics_.dead_end_sccs++;
        statistics_.pruned_dead_end_sccs++;

        assert(!expansion_infos_.empty());
        unsigned lowlink = -1;

        for (;;) {
            ExpansionInformation& exp = expansion_infos_.back();
            StackInformation& stack_info = stack_infos_[exp.stack_index];
            const StateID stateid = stack_info.state_ref;
            SearchNodeInformation& node = search_space_[stateid];
            node.lowlink = std::min(node.lowlink, lowlink);

            for (const auto& successors : exp.successors) {
                for (const StateID sid : successors.support()) {
                    SearchNodeInformation& succ_info = search_space_[sid];
                    succ_info.value = succ_info.state_cost;
                    succ_info.mark_dead_end();

                    if (succ_info.is_onstack()) {
                        node.lowlink =
                            std::min(node.lowlink, succ_info.lowlink);
                    }
                }
            }

            lowlink = node.lowlink;

            if (exp.stack_index == lowlink) {
                auto it = stack_infos_.rbegin();
                do {
                    SearchNodeInformation& info = search_space_[it->state_ref];
                    info.value = info.state_cost;
                    info.mark_dead_end();
                } while ((it++)->state_ref != stateid);

                stack_infos_.erase(it.base(), stack_infos_.end());
                expansion_infos_.pop_back();
                return;
            }

            expansion_infos_.pop_back();
        }
    }

    bool check_early_convergence(const SearchNodeInformation& node) const
    {
        if constexpr (UseInterval) {
            return node.value.upper <= node.value.lower;
        } else {
            return node.value <= cost_bound_.lower;
        }
    }
};

} // namespace exhaustive_dfs
} // namespace engines
} // namespace probfd

#endif // __EXHAUSTIVE_DFS_H__
