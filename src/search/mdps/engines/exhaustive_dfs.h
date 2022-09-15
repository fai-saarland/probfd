#ifndef MDPS_ENGINES_EXHAUSTIVE_DFS_H
#define MDPS_ENGINES_EXHAUSTIVE_DFS_H

#include "../../utils/timer.h"
#include "../engine_interfaces/heuristic_search_connector.h"
#include "../engine_interfaces/new_state_handler.h"
#include "../engine_interfaces/successor_sorting.h"
#include "../logging.h"
#include "../progress_report.h"
#include "../storage/per_state_storage.h"
#include "../value_utils.h"
#include "engine.h"
#include "heuristic_search_state_information.h"

#include <cassert>
#include <deque>
#include <limits>
#include <memory>

#define TIMERS_ENABLED(x)

namespace probabilistic {
namespace engines {

/// I do not know this algorithm.
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

    TIMERS_ENABLED(utils::Timer evaluation_time = utils::Timer(true);)

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
        TIMERS_ENABLED(out << "  Dead end evaluator time: " << evaluation_time
                           << std::endl;)
        out << "  Average dead-end SCC size: "
            << (static_cast<double>(summed_dead_end_scc_sizes) /
                static_cast<int>(dead_end_sccs))
            << std::endl;
    }

    void evaluation_started()
    {
        evaluations++;
        TIMERS_ENABLED(evaluation_time.resume();)
    }

    void evaluation_finished() { TIMERS_ENABLED(evaluation_time.stop();) }
};

enum class BacktrackingUpdateType {
    Disabled,
    SimplePropagation,
    FullValueUpdatesOnTrace,
    FullValueUpdatesOnStack,
};

inline bool update_lower_bound(value_type::value_t& x, value_type::value_t v)
{
    if (v > x) {
        x = v;
        return true;
    }

    return false;
}

inline bool
update_lower_bound(value_utils::IntervalValue& x, value_type::value_t v)
{
    if (v > x.lower) {
        x.lower = v;
        return true;
    }

    return false;
}

template <typename State, typename Action, typename DualBounds>
class ExhaustiveDepthFirstSearch
    : public MDPEngine<State, Action>
    , public heuristic_search::PerStateInformationLookup {
    using IncumbentSolution = value_utils::IncumbentSolution<DualBounds>;

    struct SearchNodeInformation {
        static constexpr uint8_t NEW = 0;
        static constexpr uint8_t CLOSED = 1;
        static constexpr uint8_t OPEN = 2;
        static constexpr uint8_t ONSTACK = 4;
        static constexpr uint8_t DEAD = 3;
        static constexpr uint8_t MARKED = 7;

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

        // TODO store lowlink in hash map -> only required for states still on
        // stack
        unsigned lowlink = -1;
        uint8_t status = NEW;
        IncumbentSolution value;
    };

public:
    explicit ExhaustiveDepthFirstSearch(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        RewardFunction<State, Action>* reward_function,
        value_utils::IntervalValue reward_bound,
        TransitionGenerator<Action>* transition_generator,
        HeuristicSearchConnector* connector,
        StateEvaluator<State>* evaluator,
        StateEvaluator<State>* dead_end_eval,
        bool reevaluate,
        bool notify_initial,
        SuccessorSorting<Action>* successor_sorting,
        BacktrackingUpdateType path_updates,
        bool only_propagate_when_changed,
        NewStateHandler<State>* new_state_handler,
        ProgressReport* progress)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              reward_bound,
              transition_generator)
        , statistics_()
        , report_(progress)
        , dead_end_value_(this->get_minimal_reward())
        , trivial_bound_(get_trivial_bound())
        , evaluator_(evaluator)
        , dead_end_evaluator_(dead_end_eval)
        , new_state_handler_(new_state_handler)
        , reverse_path_updates_(path_updates)
        , only_propagate_when_changed_(only_propagate_when_changed)
        , evaluator_recomputation_(reevaluate)
        , notify_initial_state_(notify_initial)
        , successor_sort_(successor_sorting)
    {
        connector->set_lookup_function(this);
    }

    virtual const void* lookup(const StateID& state_id) override
    {
        return &search_space_[state_id];
    }

    virtual value_type::value_t solve(const State& state) override
    {
        StateID stateid = this->get_state_id(state);
        SearchNodeInformation& info = search_space_[stateid];
        if (!initialize_search_node(state, info)) {
        } else if (!push_state(stateid, info)) {
            logging::out << "initial state is dead end!" << std::endl;
        } else {
            register_value_reports(&info);
            run_exploration();
        }

        return value_utils::as_lower_bound(info.value);
    }

    virtual bool supports_error_bound() const override { return false; }

    virtual value_type::value_t get_error(const State& s) override
    {
        if constexpr (DualBounds::value) {
            const SearchNodeInformation& info =
                search_space_[this->get_state_id(s)];
            return std::abs(info.value.error_bound());
        } else {
            return std::numeric_limits<value_type::value_t>::infinity();
        }
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    void register_value_reports(const SearchNodeInformation* info)
    {
        if constexpr (DualBounds::value) {
            this->report_->register_value("vl", [info]() {
                return info->value.lower;
            });
            this->report_->register_value("vu", [info]() {
                return info->value.upper;
            });
        } else {
            this->report_->register_value("v", [info]() {
                return info->value;
            });
        }
    }

    EvaluationResult evaluate(const State& state)
    {
        TIMERS_ENABLED(statistics_.evaluation_started();)
        const EvaluationResult res = evaluator_->operator()(state);
        TIMERS_ENABLED(statistics_.evaluation_finished();)
        return res;
    }

    bool is_dead_end(const StateID& state_id)
    {
        State state = this->lookup_state(state_id);
        return is_dead_end(state);
    }

    bool is_dead_end(const State& state)
    {
        TIMERS_ENABLED(statistics_.evaluation_started();)
        bool result = false;
        if (dead_end_evaluator_ != nullptr) {
            result = (bool)dead_end_evaluator_->operator()(state);
        }
        TIMERS_ENABLED(statistics_.evaluation_finished();)
        return result;
    }

    IncumbentSolution dead_end_value() const { return dead_end_value_; }

    IncumbentSolution get_trivial_bound() const
    {
        if constexpr (DualBounds::value) {
            return IncumbentSolution(
                this->get_minimal_reward(),
                this->get_maximal_reward());
        } else {
            return IncumbentSolution(this->get_minimal_reward());
        }
    }

private:
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
    };

    struct SCCTransition {
        Distribution<StateID> successors;
        value_type::value_t base = value_type::zero;
        value_type::value_t self_loop = value_type::zero;
    };

    struct StackInformation {
        explicit StackInformation(const StateID& state_ref)
            : state_ref(state_ref)
        {
        }

        std::vector<SCCTransition> successors;
        StateID state_ref;
        int i = -1;
    };

    bool
    initialize_search_node(const StateID& state_id, SearchNodeInformation& info)
    {
        return initialize_search_node(this->lookup_state(state_id), info);
    }

    bool initialize_search_node(const State& state, SearchNodeInformation& info)
    {
        assert(info.is_new());
        info.value = trivial_bound_;

        auto reward = this->get_state_reward(state);
        if (reward) {
            info.close();
            info.value = IncumbentSolution((value_type::value_t)reward);
            ++statistics_.goal_states;
            if (new_state_handler_) {
                new_state_handler_->touch_goal(state);
            }
            return false;
        }

        reward = evaluate(state);
        if (reward) {
            info.value = dead_end_value_;
            info.mark_dead_end();
            ++statistics_.dead_ends;
            if (new_state_handler_) {
                new_state_handler_->touch_dead_end(state);
            }
            return false;
        }

        if constexpr (DualBounds::value) {
            info.value.upper = static_cast<value_type::value_t>(reward);
        }

        info.open();
        if (new_state_handler_) {
            new_state_handler_->touch(state);
        }

        return true;
    }

    bool push_state(const StateID& state_id, SearchNodeInformation& info)
    {
        std::vector<Action> aops;
        std::vector<Distribution<StateID>> successors;
        this->generate_all_successors(state_id, aops, successors);
        if (successors.empty()) {
            info.value = dead_end_value_;
            info.set_dead_end();
            statistics_.terminal++;
            return false;
        }

        statistics_.expanded++;

        if (successor_sort_ != nullptr) {
            successor_sort_->operator()(state_id, aops, successors);
        }

        expansion_infos_.emplace_back(stack_infos_.size(), neighbors_.size());
        stack_infos_.emplace_back(state_id);

        ExpansionInformation& exp = expansion_infos_.back();
        StackInformation& si = stack_infos_.back();

        si.successors.resize(aops.size());

        const auto reward = value_utils::as_lower_bound(info.value);

        bool pure_self_loop = true;

        unsigned j = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            const auto& a = aops[i];
            auto& succs = successors[i];
            auto& t = si.successors[i];
            bool all_self_loops = true;

            for (auto it = succs.begin(); it != succs.end();) {
                const auto [succ_id, prob] = *it;

                if (succ_id == state_id) {
                    t.self_loop += prob;
                    it = succs.erase(it);
                    continue;
                }

                SearchNodeInformation& succ_info = search_space_[succ_id];
                if (succ_info.is_new()) {
                    initialize_search_node(succ_id, succ_info);
                }

                if (succ_info.is_closed()) {
                    t.base +=
                        prob * value_utils::as_lower_bound(succ_info.value);
                    exp.all_successors_are_dead =
                        exp.all_successors_are_dead && succ_info.is_dead_end();
                    exp.all_successors_marked_dead =
                        exp.all_successors_marked_dead &&
                        succ_info.is_marked_dead_end();

                    all_self_loops = false;

                    it = succs.erase(it);
                } else {
                    ++it;
                }
            }

            if (succs.empty()) {
                if (!all_self_loops) {
                    pure_self_loop = false;
                    t.base += reward + this->get_action_reward(state_id, a);
                    auto non_loop = value_type::one - t.self_loop;
                    update_lower_bound(info.value, t.base / non_loop);
                }
            } else {
                t.base += reward + this->get_action_reward(state_id, a);

                if (t.self_loop == value_type::zero) {
                    t.self_loop = value_type::one;
                } else {
                    assert(t.self_loop < value_type::one);
                    t.self_loop =
                        value_type::one / (value_type::one - t.self_loop);
                }

                if (i != j) {
                    std::swap(si.successors[i], si.successors[j]);
                    successors[i].swap(successors[j]);
                }
                ++j;
            }
        }

        if (j == 0) {
            expansion_infos_.pop_back();
            stack_infos_.pop_back();

            if (pure_self_loop) {
                info.value = dead_end_value_;
                info.set_dead_end();
                ++statistics_.self_loop;
            } else {
                info.value =
                    IncumbentSolution(value_utils::as_lower_bound(info.value));
                info.close();
            }

            return false;
        }

        successors.resize(j);
        si.successors.resize(j);
        si.i = 0;

        info.set_onstack(stack_infos_.size() - 1);
        exp.successors = std::move(successors);
        exp.succ = exp.successors.back().begin();

        return true;
    }

    void run_exploration()
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

            expanding.all_successors_are_dead =
                expanding.all_successors_are_dead && last_all_dead_;
            expanding.all_successors_marked_dead =
                expanding.all_successors_marked_dead && last_all_marked_dead_;

            if (evaluator_recomputation_ && backtracked_from_dead_end_scc_) {
                if (is_dead_end(stateid)) {
                    mark_current_component_dead();
                    continue;
                }
            }

            int t = stack_info.successors.size() - stack_info.i - 1;
            SCCTransition* inc = &stack_info.successors[t];
            bool val_changed = false;
            bool completely_explored = false;

            do {
                for (; expanding.succ != expanding.successors.back().end();
                     ++expanding.succ) {
                    const auto [succ_id, prob] = *expanding.succ;

                    assert(succ_id != stateid);
                    SearchNodeInformation& succ_info = search_space_[succ_id];
                    assert(!succ_info.is_new());

                    if (succ_info.is_open()) {
                        if (is_dead_end(succ_id)) {
                            statistics_.dead_ends++;
                            succ_info.value = dead_end_value_;
                            succ_info.mark_dead_end();
                            inc->base += prob * value_utils::as_lower_bound(
                                                    succ_info.value);
                        } else if (push_state(succ_id, succ_info)) {
                            goto skip;
                        } else {
                            expanding.all_successors_are_dead =
                                expanding.all_successors_are_dead &&
                                succ_info.is_dead_end();
                            expanding.all_successors_marked_dead =
                                expanding.all_successors_are_dead &&
                                succ_info.is_marked_dead_end();
                            inc->base += prob * value_utils::as_lower_bound(
                                                    succ_info.value);
                        }
                    } else if (succ_info.is_onstack()) {
                        node_info.lowlink =
                            std::min(node_info.lowlink, succ_info.lowlink);
                        inc->successors.add(succ_id, prob);
                    } else {
                        assert(succ_info.is_closed());
                        expanding.all_successors_are_dead =
                            expanding.all_successors_are_dead &&
                            succ_info.is_dead_end();
                        expanding.all_successors_marked_dead =
                            expanding.all_successors_are_dead &&
                            succ_info.is_marked_dead_end();
                        inc->base +=
                            prob * value_utils::as_lower_bound(succ_info.value);
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
            } while (true);

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
                        info.value = dead_end_value_;
                        info.set_dead_end();
                    } while ((rend++)->state_ref != stateid);

                    statistics_.dead_end_sccs++;
                    statistics_.summed_dead_end_scc_sizes += scc_size;
                } else {
                    unsigned scc_size = 0;
                    do {
                        auto& info = search_space_[rend->state_ref];
                        info.close();

                        if constexpr (DualBounds::value) {
                            val_changed =
                                value_utils::update(
                                    info.value,
                                    IncumbentSolution(info.value.lower)) ||
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
                                value_type::value_t best =
                                    s.successors.back().base;
                                for (int i = s.successors.size() - 1; i >= 0;
                                     --i) {
                                    const auto& t = s.successors[i];
                                    value_type::value_t t_first = t.base;
                                    for (auto [succ_id, prob] : t.successors) {
                                        t_first +=
                                            prob *
                                            value_utils::as_lower_bound(
                                                search_space_[succ_id].value);
                                    }
                                    t_first = t_first * t.self_loop;
                                    best = best > t_first ? best : t_first;
                                }

                                SearchNodeInformation& node_info =
                                    search_space_[s.state_ref];
                                if (best > value_utils::as_lower_bound(
                                               node_info.value)) {
                                    changed = changed ||
                                              !value_type::approx_equal()(
                                                  value_utils::as_lower_bound(
                                                      node_info.value),
                                                  best);
                                    node_info.value = IncumbentSolution(best);
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

            if (val_changed || !only_propagate_when_changed_) {
                switch (reverse_path_updates_) {
                case BacktrackingUpdateType::Disabled: break;
                case BacktrackingUpdateType::SimplePropagation:
                    propagate_value_along_trace(
                        completely_explored,
                        value_utils::as_lower_bound(node_info.value));
                    break;
                case BacktrackingUpdateType::FullValueUpdatesOnTrace:
                    value_updates_along_trace(completely_explored);
                    break;
                case BacktrackingUpdateType::FullValueUpdatesOnStack:
                    value_updates_along_stack(completely_explored);
                    break;
                }
            }
        }
    }

    void propagate_value_along_trace(bool was_poped, value_type::value_t val)
    {
        auto it = expansion_infos_.rbegin();
        if (!was_poped) {
            it += 2;
        }

        for (; it != expansion_infos_.rend(); ++it) {
            StackInformation& st = stack_infos_[it->stack_index];
            SearchNodeInformation& sn = search_space_[st.state_ref];
            const auto& t = st.successors[st.successors.size() - st.i - 1];
            const value_type::value_t v = t.base + it->succ->second * val;
            if (!update_lower_bound(sn.value, v)) {
                break;
            }

            val = v;
        }

        if (it == expansion_infos_.rend()) {
            report_->operator()();
        }
    }

    void value_updates_along_trace(bool /*was_poped*/)
    {
        auto it = expansion_infos_.rbegin();
        // if (!was_poped) {
        //     ++it;
        // }
        for (; it != expansion_infos_.rend(); ++it) {
            ++statistics_.value_updates;

            StackInformation& st = stack_infos_[it->stack_index];
            SearchNodeInformation& sn = search_space_[st.state_ref];
            IncumbentSolution new_val(value_utils::as_lower_bound(sn.value));
            {
                const int i = it->successors.size() - 1;
                const auto& succs = it->successors[i];
                const auto& t = st.successors[i];

                IncumbentSolution val(t.base);
                for (const auto [succ_id, prob] : t.successors) {
                    val += prob * search_space_[succ_id].value;
                }

                for (auto succ = it->succ; succ != succs.end(); ++succ) {
                    const auto& succ_info = search_space_[succ->first];
                    if (succ_info.is_new()) {
                        val += succ->second * trivial_bound_;
                    } else {
                        val += succ->second * succ_info.value;
                    }
                }

                val *= t.self_loop;
                value_utils::set_max(new_val, val);
            }

            for (int i = it->successors.size() - 2; i >= 0; --i) {
                auto& succs = it->successors[i];
                auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (const auto [succ_id, prob] : t.successors) {
                    val += prob * search_space_[succ_id].value;
                }

                for (auto succ = succs.begin(); succ != succs.end();) {
                    const auto [succ_id, prob] = *succ;
                    const auto& succ_info = search_space_[succ_id];

                    if (succ_info.is_new()) {
                        val += prob * trivial_bound_;
                        ++succ;
                    } else if (succ_info.is_onstack()) {
                        sn.lowlink = std::min(sn.lowlink, succ_info.lowlink);
                        val += prob * succ_info.value;
                        t.successors.add(succ_id, prob);
                        succ = succs.erase(succ);
                    } else {
                        assert(succ_info.is_closed());
                        t.base +=
                            value_utils::as_lower_bound(succ_info.value) * prob;
                        val += prob * succ_info.value;
                        it->all_successors_are_dead =
                            it->all_successors_are_dead &&
                            succ_info.is_dead_end();
                        it->all_successors_marked_dead =
                            it->all_successors_marked_dead &&
                            succ_info.is_marked_dead_end();
                        succ = succs.erase(succ);
                    }
                }

                val *= t.self_loop;
                value_utils::set_max(new_val, val);
            }

            for (unsigned i = st.successors.size() - 1;
                 i > it->successors.size();
                 --i) {
                const auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (const auto [succ_id, prob] : t.successors) {
                    val += prob * search_space_[succ_id].value;
                }

                val *= t.self_loop;
                value_utils::set_max(new_val, val);
            }

            if (!value_utils::update(sn.value, new_val) &&
                only_propagate_when_changed_) {
                return;
            }
        }

        assert(it == expansion_infos_.rend());
        report_->operator()();
    }

    void value_updates_along_stack(bool /*was_poped*/)
    {
        auto it = expansion_infos_.rbegin();
        // if (!was_poped) {
        //     ++it;
        // }
        int stack_index = stack_infos_.size() - 1;
        for (; it != expansion_infos_.rend(); ++it) {
            assert(stack_index <= int(it->stack_index));
            for (; stack_index > int(it->stack_index); --stack_index) {
                ++statistics_.value_updates;

                StackInformation& st = stack_infos_[stack_index];
                SearchNodeInformation& sn = search_space_[st.state_ref];
                IncumbentSolution new_val(
                    value_utils::as_lower_bound(sn.value));
                for (int i = st.successors.size() - 1; i >= 0; --i) {
                    const auto& t = st.successors[i];
                    IncumbentSolution val(t.base);
                    for (const auto [succ_id, prob] : t.successors) {
                        val += prob * search_space_[succ_id].value;
                    }

                    val *= t.self_loop;
                    value_utils::set_max(new_val, val);
                }

                value_utils::update(sn.value, new_val);
            }

            ++statistics_.value_updates;
            --stack_index;

            StackInformation& st = stack_infos_[it->stack_index];
            SearchNodeInformation& sn = search_space_[st.state_ref];
            IncumbentSolution new_val(value_utils::as_lower_bound(sn.value));
            {
                const int i = it->successors.size() - 1;
                const auto& succs = it->successors[i];
                const auto& t = st.successors[i];

                IncumbentSolution val(t.base);
                for (const auto [succ_id, prob] : t.successors) {
                    val += prob * search_space_[succ_id].value;
                }

                for (auto succ = it->succ; succ != succs.end(); ++succ) {
                    const auto& succ_info = search_space_[succ->first];
                    if (succ_info.is_new()) {
                        val += succ->second * trivial_bound_;
                    } else {
                        val += succ->second * succ_info.value;
                    }
                }

                val *= t.self_loop;
                value_utils::set_max(new_val, val);
            }

            for (int i = it->successors.size() - 2; i >= 0; --i) {
                auto& succs = it->successors[i];
                auto& t = st.successors[i];

                IncumbentSolution val(t.base);
                for (const auto [succ_id, prob] : t.successors) {
                    val += prob * search_space_[succ_id].value;
                }

                for (auto succ = succs.begin(); succ != succs.end();) {
                    const auto [succ_id, prob] = *succ;
                    const auto& succ_info = search_space_[succ_id];

                    if (succ_info.is_new()) {
                        val += prob * trivial_bound_;
                        ++succ;
                    } else if (succ_info.is_onstack()) {
                        sn.lowlink = std::min(sn.lowlink, succ_info.lowlink);
                        val += prob * succ_info.value;
                        t.successors.add(succ_id, prob);
                        succ = succs.erase(succ);
                    } else {
                        assert(succ_info.is_closed());
                        t.base +=
                            value_utils::as_lower_bound(succ_info.value) * prob;
                        val += prob * succ_info.value;
                        it->all_successors_are_dead =
                            it->all_successors_are_dead &&
                            succ_info.is_dead_end();
                        it->all_successors_marked_dead =
                            it->all_successors_marked_dead &&
                            succ_info.is_marked_dead_end();
                        succ = succs.erase(succ);
                    }
                }

                val *= t.self_loop;
                value_utils::set_max(new_val, val);
            }

            for (unsigned i = st.successors.size() - 1;
                 i > it->successors.size();
                 --i) {
                const auto& t = st.successors[i];

                IncumbentSolution val(t.base);
                for (const auto [succ_id, prob] : t.successors) {
                    val += prob * search_space_[succ_id].value;
                }

                val *= t.self_loop;
                value_utils::set_max(new_val, val);
            }

            if (!value_utils::update(sn.value, new_val) &&
                only_propagate_when_changed_) {
                return;
            }
        }

        assert(stack_index < 0);
        assert(it == expansion_infos_.rend());
        report_->operator()();
    }

    void mark_current_component_dead()
    {
        statistics_.sccs++;
        statistics_.dead_end_sccs++;
        statistics_.pruned_dead_end_sccs++;

        assert(!expansion_infos_.empty());
        unsigned lowlink = -1;

        do {
            ExpansionInformation& exp = expansion_infos_.back();
            StackInformation& stack_info = stack_infos_[exp.stack_index];
            const StateID stateid = stack_info.state_ref;
            SearchNodeInformation& node = search_space_[stateid];
            node.lowlink = std::min(node.lowlink, lowlink);

            for (const auto& successors : exp.successors) {
                for (const StateID sid : successors.elements()) {
                    SearchNodeInformation& succ_info = search_space_[sid];
                    succ_info.value = dead_end_value_;
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
                    info.value = dead_end_value_;
                    info.mark_dead_end();
                } while ((it++)->state_ref != stateid);

                stack_infos_.erase(it.base(), stack_infos_.end());
                expansion_infos_.pop_back();
                break;
            }

            expansion_infos_.pop_back();
        } while (true);
    }

    bool check_early_convergence(const SearchNodeInformation& node) const
    {
        if constexpr (DualBounds::value) {
            return node.value.lower >= node.value.upper;
        } else {
            return value_utils::as_lower_bound(node.value) >=
                   this->get_maximal_reward();
        }
    }

    Statistics statistics_;

    ProgressReport* report_;
    const IncumbentSolution dead_end_value_;
    const IncumbentSolution trivial_bound_;

    StateEvaluator<State>* evaluator_;
    StateEvaluator<State>* dead_end_evaluator_;
    NewStateHandler<State>* new_state_handler_;

    const BacktrackingUpdateType reverse_path_updates_;
    const bool only_propagate_when_changed_;
    const bool evaluator_recomputation_;
    const bool notify_initial_state_;

    SuccessorSorting<Action>* successor_sort_;

    storage::PerStateStorage<SearchNodeInformation> search_space_;

    std::deque<ExpansionInformation> expansion_infos_;
    std::deque<StackInformation> stack_infos_;
    std::vector<StateID> neighbors_;

    bool last_all_dead_ = true;
    bool last_all_marked_dead_ = true;
    bool backtracked_from_dead_end_scc_ = false;
};

} // namespace exhaustive_dfs
} // namespace engines
} // namespace probabilistic

#undef TIMERS_ENABLED

#endif // __EXHAUSTIVE_DFS_H__