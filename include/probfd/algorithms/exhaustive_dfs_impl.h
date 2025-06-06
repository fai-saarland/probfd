#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H
#error "This file should only be included from exhaustive_dfs.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/algorithms/transition_sorter.h"

#include "probfd/utils/not_implemented.h"

#include "probfd/heuristic.h"
#include "probfd/transition_tail.h"

#include <cassert>
#include <ranges>

namespace probfd::algorithms::exhaustive_dfs {

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

inline void Statistics::print(std::ostream& out) const
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
    out << "  Partially pruned " << pruned_dead_end_sccs << " dead-end SCC(s)."
        << std::endl;
    out << "  Average dead-end SCC size: "
        << (static_cast<double>(summed_dead_end_scc_sizes) /
            static_cast<int>(dead_end_sccs))
        << std::endl;
}

template <typename State, typename Action, bool UseInterval>
ExhaustiveDepthFirstSearch<State, Action, UseInterval>::
    ExhaustiveDepthFirstSearch(
        value_t epsilon,
        std::shared_ptr<TransitionSorterType> transition_sorting,
        Interval cost_bound,
        bool path_updates,
        bool only_propagate_when_changed)
    : IterativeMDPAlgorithm<State, Action>(epsilon)
    , transition_sort_(transition_sorting)
    , cost_bound_(cost_bound)
    , trivial_bound_([=] {
        if constexpr (UseInterval) {
            return cost_bound;
        } else {
            return cost_bound.upper;
        }
    }())
    , value_propagation_(path_updates)
    , only_propagate_when_changed_(only_propagate_when_changed)
{
}

template <typename State, typename Action, bool UseInterval>
Interval ExhaustiveDepthFirstSearch<State, Action, UseInterval>::solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
    ProgressReport progress,
    double)
{
    StateID stateid = mdp.get_state_id(state);
    SearchNodeInfo& info = search_space_[stateid];
    if (!initialize_search_node(mdp, heuristic, state, info)) {
        return search_space_.lookup_bounds(stateid);
    }

    if (!push_state(mdp, heuristic, stateid, info)) {
        std::cout << "initial state is dead end!" << std::endl;
        return search_space_.lookup_bounds(stateid);
    }

    register_value_reports(info, progress);
    run_exploration(mdp, heuristic, progress);

    return search_space_.lookup_bounds(stateid);
}

template <typename State, typename Action, bool UseInterval>
auto ExhaustiveDepthFirstSearch<State, Action, UseInterval>::compute_policy(
    MDPType&,
    HeuristicType&,
    ParamType<State>,
    ProgressReport,
    double) -> std::unique_ptr<PolicyType>
{
    not_implemented();
}

template <typename State, typename Action, bool UseInterval>
void ExhaustiveDepthFirstSearch<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
void ExhaustiveDepthFirstSearch<State, Action, UseInterval>::
    register_value_reports(const SearchNodeInfo& info, ProgressReport& progress)
{
    progress.register_bound("v", [info]() {
        if constexpr (UseInterval) {
            return info.value;
        } else {
            return Interval(info.value, INFINITE_VALUE);
        }
    });
}

template <typename State, typename Action, bool UseInterval>
bool ExhaustiveDepthFirstSearch<State, Action, UseInterval>::
    initialize_search_node(
        MDPType& mdp,
        HeuristicType& heuristic,
        StateID state_id,
        SearchNodeInfo& info)
{
    return initialize_search_node(
        mdp,
        heuristic,
        mdp.get_state(state_id),
        info);
}

template <typename State, typename Action, bool UseInterval>
bool ExhaustiveDepthFirstSearch<State, Action, UseInterval>::
    initialize_search_node(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        SearchNodeInfo& info)
{
    assert(info.is_new());
    info.value = trivial_bound_;

    TerminationInfo term_info = mdp.get_termination_info(state);
    const value_t term_cost = term_info.get_cost();
    info.term_cost = term_cost;

    if (term_info.is_goal_state()) {
        info.close();
        info.value = AlgorithmValueType(term_cost);
        ++statistics_.goal_states;
        return false;
    }

    const value_t estimate = heuristic.evaluate(state);
    if (estimate == term_cost) {
        info.value = AlgorithmValueType(term_cost);
        info.mark_dead_end();
        ++statistics_.dead_ends;
        return false;
    }

    if constexpr (UseInterval) {
        info.value.lower = estimate;
    }

    info.open();

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool ExhaustiveDepthFirstSearch<State, Action, UseInterval>::push_state(
    MDPType& mdp,
    HeuristicType& heuristic,
    StateID state_id,
    SearchNodeInfo& info)
{
    std::vector<Action> aops;
    std::vector<SuccessorDistribution> successor_dists;
    const State state = mdp.get_state(state_id);
    mdp.generate_all_transitions(state, aops, successor_dists);
    if (successor_dists.empty()) {
        info.value = AlgorithmValueType(info.term_cost);
        info.set_dead_end();
        statistics_.terminal++;
        return false;
    }

    statistics_.expanded++;

    if (transition_sort_ != nullptr) {
        transition_sort_->sort(state, aops, successor_dists, search_space_);
    }

    expansion_infos_.emplace_back(stack_infos_.size());
    stack_infos_.emplace_back(state_id);

    ExpansionInformation& exp = expansion_infos_.back();
    StackInformation& si = stack_infos_.back();

    si.successors.resize(aops.size());

    bool pure_self_loop = true;

    unsigned j = 0;
    for (unsigned i = 0; i < aops.size(); ++i) {
        auto& succs = successor_dists[i];
        auto& t = si.successors[i];
        const auto& a = aops[i];

        if (succs.non_source_successor_dist.empty()) {
            continue;
        }

        pure_self_loop = false;

        if (succs.non_source_probability == 0_vt) {
            update_lower_bound(info.value, mdp.get_action_cost(a));
            continue;
        }

        t.normalization = 1_vt / succs.non_source_probability;
        t.closed_value = mdp.get_action_cost(a);

        bool all_closed = true;

        for (auto& elem : succs.non_source_successor_dist) {
            const auto [succ_id, prob] = elem;

            SearchNodeInfo& succ_info = search_space_[succ_id];
            if (succ_info.is_new()) {
                initialize_search_node(mdp, heuristic, succ_id, succ_info);
            }

            if (succ_info.is_closed()) {
                t.closed_value += prob * succ_info.get_value();
                exp.update_successors_dead(succ_info.is_dead_end());
                exp.all_successors_marked_dead =
                    exp.all_successors_marked_dead &&
                    succ_info.is_marked_dead_end();
                continue;
            }

            all_closed = false;
        }

        if (all_closed) {
            update_lower_bound(info.value, t.closed_value * t.normalization);
            continue;
        }

        if (i != j) {
            si.successors[j] = std::move(si.successors[i]);
            successor_dists[j] = std::move(successor_dists[i]);
        }
        ++j;
    }

    if (j == 0) {
        expansion_infos_.pop_back();
        stack_infos_.pop_back();

        if (pure_self_loop) {
            info.value = AlgorithmValueType(info.term_cost);
            info.set_dead_end();
            ++statistics_.self_loop;
        } else {
            info.value = AlgorithmValueType(info.get_value());
            info.close();
        }

        return false;
    }

    successor_dists.erase(successor_dists.begin() + j, successor_dists.end());
    si.successors.erase(si.successors.begin() + j, si.successors.end());
    si.i = 0;

    info.set_onstack(stack_infos_.size() - 1);
    exp.successors = std::move(successor_dists);
    exp.succ = exp.successors.back().non_source_successor_dist.begin();

    return true;
}

template <typename State, typename Action, bool UseInterval>
void ExhaustiveDepthFirstSearch<State, Action, UseInterval>::run_exploration(
    MDPType& mdp,
    HeuristicType& heuristic,
    ProgressReport& progress)
{
    using namespace std;

    while (!expansion_infos_.empty()) {
        ExpansionInformation& expanding = expansion_infos_.back();
        assert(expanding.stack_index < stack_infos_.size());
        assert(!expanding.successors.empty());
        assert(
            expanding.succ !=
            expanding.successors.back().non_source_successor_dist.end());

        StackInformation& stack_info = stack_infos_[expanding.stack_index];
        assert(!stack_info.successors.empty());

        const StateID stateid = stack_info.state_ref;
        SearchNodeInfo& node_info = search_space_[stateid];

        expanding.update_successors_dead(last_all_dead_);
        expanding.all_successors_marked_dead =
            expanding.all_successors_marked_dead && last_all_marked_dead_;

        int idx = stack_info.successors.size() - stack_info.i - 1;
        SCCTransition* inc = &stack_info.successors[idx];
        bool val_changed = false;
        bool completely_explored = false;

        for (;;) {
            for (; expanding.succ !=
                   expanding.successors.back().non_source_successor_dist.end();
                 ++expanding.succ) {
                const auto [succ_id, prob] = *expanding.succ;

                assert(succ_id != stateid);
                SearchNodeInfo& succ_info = search_space_[succ_id];
                assert(!succ_info.is_new());

                if (succ_info.is_open()) {
                    if (push_state(mdp, heuristic, succ_id, succ_info)) {
                        goto skip;
                    }

                    expanding.update_successors_dead(succ_info.is_dead_end());
                    expanding.all_successors_marked_dead =
                        expanding.all_successors_are_dead &&
                        succ_info.is_marked_dead_end();
                    inc->closed_value += prob * succ_info.get_value();
                } else if (succ_info.is_onstack()) {
                    node_info.lowlink =
                        std::min(node_info.lowlink, succ_info.lowlink);
                    inc->successors.add_probability(succ_id, prob);
                } else {
                    assert(succ_info.is_closed());
                    expanding.update_successors_dead(succ_info.is_dead_end());
                    expanding.all_successors_marked_dead =
                        expanding.all_successors_are_dead &&
                        succ_info.is_marked_dead_end();
                    inc->closed_value += prob * succ_info.get_value();
                }
            }

            expanding.successors.pop_back();
            if (update_lower_bound(
                    node_info.value,
                    inc->closed_value * inc->normalization)) {
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

            expanding.succ =
                expanding.successors.back().non_source_successor_dist.begin();
        }

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
                    info.value = AlgorithmValueType(info.term_cost);
                    info.set_dead_end();
                } while ((rend++)->state_ref != stateid);

                statistics_.dead_end_sccs++;
                statistics_.summed_dead_end_scc_sizes += scc_size;
            } else {
                unsigned scc_size = 0;
                do {
                    auto& info = search_space_[rend->state_ref];
                    info.close();

                    const auto val_upd = update(
                        info.value,
                        AlgorithmValueType(as_lower_bound(info.value)),
                        this->epsilon);

                    if (val_upd.changed) val_changed = true;

                    ++scc_size;
                } while ((rend++)->state_ref != stateid);

                if (scc_size > 1) {
                    unsigned iterations = 0;
                    bool changed;
                    do {
                        changed = false;
                        for (auto it = stack_infos_.rbegin(); it != rend;
                             ++it) {
                            StackInformation& s = *it;
                            assert(!s.successors.empty());
                            value_t best = s.successors.back().closed_value;
                            for (const auto& t :
                                 std::views::reverse(s.successors)) {
                                value_t t_first = t.closed_value;
                                for (auto [succ_id, prob] : t.successors) {
                                    t_first +=
                                        prob *
                                        search_space_[succ_id].get_value();
                                }
                                t_first = t_first * t.normalization;
                                best = best > t_first ? best : t_first;
                            }

                            SearchNodeInfo& snode_info =
                                search_space_[s.state_ref];
                            if (best > snode_info.get_value()) {
                                changed = changed || !is_approx_equal(
                                                         snode_info.get_value(),
                                                         best,
                                                         this->epsilon);
                                snode_info.value = AlgorithmValueType(best);
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

        if ((val_changed || !only_propagate_when_changed_) &&
            value_propagation_) {
            propagate_value_along_trace(
                completely_explored,
                node_info.get_value(),
                progress);
        }
    }
}

template <typename State, typename Action, bool UseInterval>
void ExhaustiveDepthFirstSearch<State, Action, UseInterval>::
    propagate_value_along_trace(
        bool was_poped,
        value_t val,
        ProgressReport& progress)
{
    auto it = expansion_infos_.rbegin();
    if (!was_poped) {
        it += 2;
    }

    for (; it != expansion_infos_.rend(); ++it) {
        StackInformation& st = stack_infos_[it->stack_index];
        SearchNodeInfo& sn = search_space_[st.state_ref];
        const auto& t = st.successors[st.successors.size() - st.i - 1];
        const value_t v = t.closed_value + it->succ->probability * val;
        if (!update_lower_bound(sn.value, v)) {
            break;
        }

        val = v;
    }

    if (it == expansion_infos_.rend()) {
        progress.print();
    }
}

template <typename State, typename Action, bool UseInterval>
bool ExhaustiveDepthFirstSearch<State, Action, UseInterval>::
    check_early_convergence(const SearchNodeInfo& node) const
{
    if constexpr (UseInterval) {
        return node.value.upper <= node.value.lower;
    } else {
        return node.value <= cost_bound_.lower;
    }
}

} // namespace probfd::algorithms::exhaustive_dfs
