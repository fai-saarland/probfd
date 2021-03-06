#pragma once

#include "../../utils/timer.h"
#include "../engine_interfaces/dead_end_listener.h"
#include "../engine_interfaces/heuristic_search_connector.h"
#include "../engine_interfaces/new_state_handler.h"
#include "../engine_interfaces/successor_sorting.h"
#include "../logging.h"
#include "../progress_report.h"
#include "../storage/per_state_storage.h"
#include "engine.h"
#include "heuristic_search_state_information.h"
#include "value_utils.h"

#include <cassert>
#include <deque>
#include <memory>

#define TIMER_STATISTICS_ENABLED(x)

namespace probabilistic {
namespace exhaustive_dfs {

class Statistics {
public:
    Statistics()
    {
        TIMER_STATISTICS_ENABLED(notification_time.stop();)
        TIMER_STATISTICS_ENABLED(evaluation_time.stop();)
    }

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
        TIMER_STATISTICS_ENABLED(out << "  Dead end evaluator time: "
                                     << evaluation_time << std::endl;)
        out << "  Dead end notifications: " << notifications << std::endl;
        TIMER_STATISTICS_ENABLED(out << "  Total dead end nofication time: "
                                     << (notification_time) << std::endl;)
        out << "  Average dead-end SCC size: "
            << (static_cast<double>(summed_dead_end_scc_sizes)
                / static_cast<int>(dead_end_sccs))
            << std::endl;
        out << "  Average dead-end SCC size for notifications: "
            << (static_cast<double>(summed_notified_scc_sizes)
                / static_cast<int>(notifications))
            << std::endl;
    }

    void evaluation_started()
    {
        evaluations++;
        TIMER_STATISTICS_ENABLED(evaluation_time.resume();)
    }

    void evaluation_finished()
    {
        TIMER_STATISTICS_ENABLED(evaluation_time.stop();)
    }

    void notification_started(unsigned scc_size)
    {
        notifications++;
        TIMER_STATISTICS_ENABLED(notification_time.resume();)
        summed_notified_scc_sizes += scc_size;
    }

    void notification_finished()
    {
        TIMER_STATISTICS_ENABLED(notification_time.stop();)
    }

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
    unsigned long long notifications = 0;
    unsigned long long summed_dead_end_scc_sizes = 0;
    unsigned long long summed_notified_scc_sizes = 0;

    TIMER_STATISTICS_ENABLED(::utils::Timer evaluation_time;)
    TIMER_STATISTICS_ENABLED(::utils::Timer notification_time;)
};

enum class BacktrackingUpdateType {
    Disabled,
    SimplePropagation,
    FullValueUpdatesOnTrace,
    FullValueUpdatesOnStack,
};

struct SearchNodeCoreInformation {
    static const uint8_t NEW = 0;
    static const uint8_t CLOSED = 1;
    static const uint8_t OPEN = 2;
    static const uint8_t ONSTACK = 4;
    static const uint8_t DEAD = 3;
    static const uint8_t MARKED = 7;

    bool is_new() const { return status == NEW; }

    bool is_open() const { return status == OPEN; }

    bool is_onstack() const { return status == ONSTACK; }

    bool is_closed() const { return (status & CLOSED); }

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

    // TODO store lowlink in hash map -> only required for states still on stack
    unsigned lowlink = -1;
    uint8_t status = NEW;
};

template<typename Dual = std::false_type>
struct SearchNodeInformation : public heuristic_search::StatesValues<Dual>,
                               public SearchNodeCoreInformation {
};

inline void
set_upper_bound(
    heuristic_search::StatesValues<std::false_type>&,
    const value_type::value_t&)
{
}

inline void
set_upper_bound(
    heuristic_search::StatesValues<std::true_type>& x,
    const value_type::value_t& v)
{
    x.value2 = v;
}

inline bool
update_lower_bound(
    heuristic_search::StatesValues<std::false_type>& x,
    const value_type::value_t& v)
{
    if (v > x.value) {
        x.value = v;
        return true;
    }
    return false;
}

inline bool
update_lower_bound(
    heuristic_search::StatesValues<std::true_type>& x,
    const value_type::value_t& v)
{
    if (v > x.value) {
        x.value = v;
        return true;
    }
    return false;
}

inline void
set_lower_bound(
    heuristic_search::StatesValues<std::false_type>& x,
    const value_type::value_t& v)
{
    x.value = v;
}

inline void
set_lower_bound(
    heuristic_search::StatesValues<std::true_type>& x,
    const value_type::value_t& v)
{
    x.value = v;
}

inline const value_type::value_t&
get_lower_bound(
    const heuristic_search::StatesValues<std::false_type>& state_value)
{
    return state_value.value;
}

inline const value_type::value_t&
get_lower_bound(
    const heuristic_search::StatesValues<std::true_type>& state_value)
{
    return state_value.value;
}

template<typename State, typename Action, typename DualV>
class ExhaustiveDepthFirstSearch
    : public MDPEngine<State, Action>,
      public heuristic_search::PerStateInformationLookup {
public:
    using DualBounds = DualV;
    using SearchNodeInformation =
        typename probabilistic::exhaustive_dfs::SearchNodeInformation<
            DualBounds>;

    using IncumbentSolution = value_utils::IncumbentSolution<DualBounds>;

    template<typename... Args>
    explicit ExhaustiveDepthFirstSearch(
        HeuristicSearchConnector* connector,
        StateEvaluator<State>* evaluator,
        StateEvaluator<State>* dead_end_eval,
        DeadEndListener<State, Action>* dead_end_listener,
        bool reevaluate,
        bool notify_initial,
        SuccessorSorting<Action>* successor_sorting,
        BacktrackingUpdateType path_updates,
        bool only_propagate_when_changed,
        NewStateHandler<State>* new_state_handler,
        ProgressReport* progress,
        Args... args)
        : MDPEngine<State, Action>(args...)
        , statistics_()
        , report_(progress)
        , dead_end_value_(this->get_minimal_reward())
        , trivial_bound_(get_trivial_bound())
        , evaluator_(evaluator)
        , dead_end_evaluator_(dead_end_eval)
        , dead_end_listener_(dead_end_listener)
        , new_state_handler_(new_state_handler)
        , reverse_path_updates_(path_updates)
        , only_propagate_when_changed_(only_propagate_when_changed)
        , evaluator_recomputation_(reevaluate)
        , notify_initial_state_(notify_initial)
        , successor_sort_(successor_sorting)
        , search_space_()
        , notify_dead_ends_(dead_end_listener != nullptr)
        , store_neighbors_(
              dead_end_listener_ != nullptr
              && dead_end_listener_->requires_neighbors())
    {
        connector->set_lookup_function(this);
    }

    virtual const void* lookup(const StateID& state_id) override
    {
        return &search_space_[state_id];
    }

    virtual void solve(const State& state) override
    {
        StateID stateid = this->get_state_id(state);
        SearchNodeInformation& info = search_space_[stateid];
        if (!initialize_search_node(state, info)) {
        } else if (!push_state(stateid, info)) {
            logging::out << "initial state is dead end!" << std::endl;
        } else {
            register_value_reports(DualBounds(), &info);
            run_exploration();
        }
    }

    virtual value_type::value_t get_result(const State& s) override
    {
        const SearchNodeInformation& info =
            search_space_[this->get_state_id(s)];
        return info.value;
    }

    virtual bool supports_error_bound() const override { return false; }

    virtual value_type::value_t get_error(const State& s) override
    {
        const SearchNodeInformation& info =
            search_space_[this->get_state_id(s)];
        return std::abs(info.error_bound());
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    void register_value_reports(
        const std::true_type&,
        const SearchNodeInformation* info)
    {
        this->report_->register_value("vl", [info]() { return info->value; });
        this->report_->register_value("vu", [info]() { return info->value2; });
    }

    void register_value_reports(
        const std::false_type&,
        const SearchNodeInformation* info)
    {
        this->report_->register_value("v", [info]() { return info->value; });
    }

    EvaluationResult evaluate(const State& state)
    {
        TIMER_STATISTICS_ENABLED(statistics_.evaluation_started();)
        const EvaluationResult res = evaluator_->operator()(state);
        TIMER_STATISTICS_ENABLED(statistics_.evaluation_finished();)
        return res;
    }

    bool is_dead_end(const StateID& state_id)
    {
        State state = this->lookup_state(state_id);
        return is_dead_end(state);
    }

    bool is_dead_end(const State& state)
    {
        TIMER_STATISTICS_ENABLED(statistics_.evaluation_started();)
        bool result = false;
        if (dead_end_evaluator_ != nullptr) {
            result = (bool)dead_end_evaluator_->operator()(state);
        }
        TIMER_STATISTICS_ENABLED(statistics_.evaluation_finished();)
        return result;
    }

    IncumbentSolution dead_end_value() const
    {
        return IncumbentSolution(dead_end_value_.first);
    }

    IncumbentSolution get_trivial_bound() const
    {
        if constexpr (DualBounds::value) {
            return IncumbentSolution(
                this->get_minimal_reward(), this->get_maximal_reward());
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
        value_type::value_t base = 0;
        value_type::value_t self_loop = 0;

        void reset()
        {
            base = 0;
            self_loop = 0;
        }
    };

    struct StackInformation {
        explicit StackInformation(const StateID& state_ref)
            : state_ref(state_ref)
        {
        }

        bool operator==(const StateID& state) const
        {
            return state_ref == state;
        }

        std::vector<SCCTransition> successors;
        StateID state_ref;
        int i = -1;
    };

    bool
    initialize_search_node(const StateID& state_id, SearchNodeInformation& info)
    {
        State state = this->lookup_state(state_id);
        return initialize_search_node(state, info);
    }

    bool initialize_search_node(const State& state, SearchNodeInformation& info)
    {
        assert(info.is_new());
        auto reward = this->get_state_reward(state);
        info.set_value(trivial_bound_);
        if (reward) {
            info.close();
            info.set_value(IncumbentSolution((value_type::value_t)reward));
            ++statistics_.goal_states;
            if (new_state_handler_) {
                new_state_handler_->touch_goal(state);
            }
            return false;
        } else {
            reward = evaluate(state);
            if (reward) {
                info.set_value(dead_end_value_);
                info.mark_dead_end();
                ++statistics_.dead_ends;
                if (new_state_handler_) {
                    new_state_handler_->touch_dead_end(state);
                }
                return false;
            } else {
                set_upper_bound(info, (value_type::value_t)reward);
                info.open();
                if (new_state_handler_) {
                    new_state_handler_->touch(state);
                }
            }
        }
        return true;
    }

    bool push_state(const StateID& state_ref, SearchNodeInformation& info)
    {
        std::vector<Action> aops;
        std::vector<Distribution<StateID>> successors;
        this->generate_all_successors(state_ref, aops, successors);
        if (successors.empty()) {
            info.set_value(dead_end_value_);
            info.set_dead_end();
            statistics_.terminal++;
            return false;
        }

        statistics_.expanded++;

        if (successor_sort_ != nullptr) {
            successor_sort_->operator()(state_ref, aops, successors);
        }

        expansion_infos_.emplace_back(stack_infos_.size(), neighbors_.size());
        stack_infos_.emplace_back(state_ref);

        ExpansionInformation& exp = expansion_infos_.back();
        StackInformation& si = stack_infos_.back();

        si.successors.resize(aops.size());

        const value_type::value_t reward = get_lower_bound(info);

        bool pure_self_loop = true;

        unsigned j = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            auto& succs = successors[i];
            auto& t = si.successors[i];
            bool all_self_loops = true;
            for (auto it = succs.begin(); it != succs.end();) {
                if (it->first == state_ref) {
                    t.self_loop += it->second;
                    it = succs.erase(it);
                } else {
                    SearchNodeInformation& succ_info = search_space_[it->first];
                    if (succ_info.is_new()) {
                        initialize_search_node(it->first, succ_info);
                    }
                    if (succ_info.is_closed()) {
                        t.base += succ_info.get_value() * it->second;
                        exp.all_successors_are_dead =
                            exp.all_successors_are_dead
                            && succ_info.is_dead_end();
                        exp.all_successors_marked_dead =
                            exp.all_successors_marked_dead
                            && succ_info.is_marked_dead_end();
                        if (notify_dead_ends_ && store_neighbors_
                            && succ_info.is_marked_dead_end()) {
                            neighbors_.push_back(it->first);
                        }
                        it = succs.erase(it);
                        all_self_loops = false;
                    } else {
                        ++it;
                    }
                }
            }
            if (succs.empty()) {
                if (!all_self_loops) {
                    pure_self_loop = false;
                    t.base +=
                        reward + this->get_action_reward(state_ref, aops[i]);
                    update_lower_bound(
                        info,
                        t.base * value_type::value_t(1)
                            / (value_type::value_t(1) - t.self_loop));
                }
            } else {
                t.base += reward + this->get_action_reward(state_ref, aops[i]);
                if (t.self_loop == 0) {
                    t.self_loop = 1;
                } else {
                    assert(t.self_loop < 1);
                    t.self_loop = value_type::value_t(1)
                        / (value_type::value_t(1) - t.self_loop);
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
                info.set_value(dead_end_value_);
                info.set_dead_end();
                ++statistics_.self_loop;
                return false;
            } else {
                info.set_value(IncumbentSolution(info.get_value()));
                info.close();
                return false;
            }
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
            SearchNodeInformation& node_info =
                search_space_[stack_info.state_ref];

            expanding.all_successors_are_dead =
                expanding.all_successors_are_dead && last_all_dead_;
            expanding.all_successors_marked_dead =
                expanding.all_successors_marked_dead && last_all_marked_dead_;

            if (evaluator_recomputation_ && backtracked_from_dead_end_scc_) {
                if (is_dead_end(stack_info.state_ref)) {
                    mark_current_component_dead();
                    continue;
                }
            }

            SCCTransition* inc =
                &stack_info.successors
                     [stack_info.successors.size() - stack_info.i - 1];
            bool val_changed = false;
            bool completely_explored = false;
            do {
                for (; expanding.succ != expanding.successors.back().end();
                     ++expanding.succ) {
                    assert(expanding.succ->first != stack_info.state_ref);
                    SearchNodeInformation& succ_info =
                        search_space_[expanding.succ->first];
                    assert(!succ_info.is_new());
                    if (succ_info.is_open()) {
                        if (is_dead_end(expanding.succ->first)) {
                            statistics_.dead_ends++;
                            succ_info.set_value(dead_end_value_);
                            succ_info.mark_dead_end();
                            inc->base +=
                                (expanding.succ->second
                                 * succ_info.get_value());
                            if (notify_dead_ends_ && store_neighbors_
                                && succ_info.is_marked_dead_end()) {
                                neighbors_.push_back(expanding.succ->first);
                            }
                        } else if (push_state(
                                       expanding.succ->first, succ_info)) {
                            break;
                        } else {
                            expanding.all_successors_are_dead =
                                expanding.all_successors_are_dead
                                && succ_info.is_dead_end();
                            expanding.all_successors_marked_dead =
                                expanding.all_successors_are_dead
                                && succ_info.is_marked_dead_end();
                            inc->base +=
                                (expanding.succ->second
                                 * succ_info.get_value());
                            if (notify_dead_ends_ && store_neighbors_
                                && succ_info.is_marked_dead_end()) {
                                neighbors_.push_back(expanding.succ->first);
                            }
                        }
                    } else if (succ_info.is_onstack()) {
                        node_info.lowlink =
                            std::min(node_info.lowlink, succ_info.lowlink);
                        inc->successors.add(
                            expanding.succ->first, expanding.succ->second);
                    } else {
                        assert(succ_info.is_closed());
                        expanding.all_successors_are_dead =
                            expanding.all_successors_are_dead
                            && succ_info.is_dead_end();
                        expanding.all_successors_marked_dead =
                            expanding.all_successors_are_dead
                            && succ_info.is_marked_dead_end();
                        inc->base +=
                            (expanding.succ->second * succ_info.get_value());
                        if (notify_dead_ends_ && store_neighbors_
                            && succ_info.is_marked_dead_end()) {
                            neighbors_.push_back(expanding.succ->first);
                        }
                    }
                }
                if (expanding.succ == expanding.successors.back().end()) {
                    expanding.successors.pop_back();
                    if (update_lower_bound(
                            node_info, inc->base * inc->self_loop)) {
                        val_changed = true;
                        if (check_early_convergence(DualBounds(), node_info)) {
                            expanding.successors.clear();
                        }
                    }
                    if (expanding.successors.empty()) {
                        if (inc->successors.empty()) {
                            if (stack_info.i > 0)
                                std::swap(stack_info.successors.back(), *inc);
                            stack_info.successors.pop_back();
                        }
                        completely_explored = true;
                        break;
                    } else {
                        if (inc->successors.empty()) {
                            if (stack_info.i > 0)
                                std::swap(stack_info.successors.back(), *inc);
                            stack_info.successors.pop_back();
                            inc = &stack_info.successors
                                       [stack_info.successors.size()
                                        - stack_info.i - 1];
                        } else {
                            --inc;
                            ++stack_info.i;
                        }
                        expanding.succ = expanding.successors.back().begin();
                    }
                } else {
                    break;
                }
            } while (true);

            backtracked_from_dead_end_scc_ = false;
            last_all_dead_ = true;
            last_all_marked_dead_ = true;
            if (completely_explored) {
                last_all_dead_ = expanding.all_successors_are_dead;
                last_all_marked_dead_ = expanding.all_successors_marked_dead;
                statistics_.backtracks++;
                if (expanding.stack_index == node_info.lowlink) {
                    ++statistics_.sccs;
                    auto rend = stack_infos_.rbegin();
                    if (expanding.all_successors_are_dead) {
                        unsigned scc_size = 0;
                        while (true) {
                            scc_size++;
                            SearchNodeInformation& info =
                                search_space_[rend->state_ref];
                            info.set_value(dead_end_value_);
                            info.set_dead_end();
                            if ((rend++)->state_ref == stack_info.state_ref) {
                                break;
                            }
                        }
                        statistics_.dead_end_sccs++;
                        statistics_.summed_dead_end_scc_sizes += scc_size;
                        if (notify_dead_ends_
                            && (expanding.all_successors_marked_dead
                                || !dead_end_listener_->requires_neighbors())
                            && (notify_initial_state_
                                || expansion_infos_.size() > 1)) {
                            statistics_.notification_started(scc_size);

                            std::deque<StateID> component;
                            for (auto sit = stack_infos_.rbegin(); sit != rend;
                                 ++sit) {
                                component.push_back(sit->state_ref);
                            }

                            if (dead_end_listener_->operator()(
                                    component.begin(),
                                    component.end(),
                                    neighbors_.begin(),
                                    neighbors_.end())) {
                                for (auto it = stack_infos_.rbegin();
                                     it != rend;
                                     ++it) {
                                    search_space_[it->state_ref]
                                        .mark_dead_end();
                                }
                                backtracked_from_dead_end_scc_ = true;
                            } else {
                                notify_dead_ends_ = false;
                                store_neighbors_ = false;
                                std::deque<StateID>().swap(neighbors_);
                            }

                            statistics_.notification_finished();
                        }
                    } else {
                        unsigned scc_size = 0;
                        do {
                            SearchNodeInformation& info =
                                search_space_[rend->state_ref];
                            info.close();
                            if (DualBounds::value) {
                                val_changed =
                                    info.update_value(
                                        true,
                                        IncumbentSolution(info.get_value()))
                                    || val_changed;
                            }
                            ++scc_size;
                            if (rend->state_ref == stack_info.state_ref) {
                                ++rend;
                                break;
                            }
                            ++rend;
                        } while (true);

                        if (scc_size > 1) {
                            unsigned iterations = 0;
                            bool changed = true;
                            do {
                                changed = false;
                                for (auto it = stack_infos_.rbegin();
                                     it != rend;
                                     ++it) {
                                    StackInformation& s = *it;
                                    assert(!s.successors.empty());
                                    value_type::value_t best =
                                        s.successors.back().base;
                                    for (int i = s.successors.size() - 1;
                                         i >= 0;
                                         --i) {
                                        const auto& t = s.successors[i];
                                        value_type::value_t t_first = t.base;
                                        for (auto succ = t.successors.begin();
                                             succ != t.successors.end();
                                             ++succ) {
                                            t_first +=
                                                search_space_[succ->first].value
                                                * succ->second;
                                        }
                                        t_first = t_first * t.self_loop;
                                        best = best > t_first ? best : t_first;
                                    }
                                    SearchNodeInformation& node_info =
                                        search_space_[s.state_ref];
                                    if (best > node_info.value) {
                                        changed = changed
                                            || !value_type::approx_equal()(
                                                      node_info.value, best);
                                        node_info.set_value(
                                            IncumbentSolution(best));
                                    }
                                }
                                ++iterations;
                            } while (changed);
                            val_changed = val_changed || iterations > 1;
                        }
                    }

                    if (store_neighbors_) {
                        neighbors_.resize(expanding.neighbors_size);
                        if (node_info.is_marked_dead_end()) {
                            neighbors_.push_back(stack_info.state_ref);
                        }
                    }

                    stack_infos_.erase((rend.base()), stack_infos_.end());
                }
                expansion_infos_.pop_back();
            }

            if (val_changed || !only_propagate_when_changed_) {
                switch (reverse_path_updates_) {
                case BacktrackingUpdateType::Disabled:
                    break;
                case BacktrackingUpdateType::SimplePropagation:
                    propagate_value_along_trace(
                        completely_explored, node_info.get_value());
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
            ++it;
            ++it;
        }
        for (; it != expansion_infos_.rend(); ++it) {
            StackInformation& st = stack_infos_[it->stack_index];
            SearchNodeInformation& sn = search_space_[st.state_ref];
            const auto& t = st.successors[st.successors.size() - st.i - 1];
            const value_type::value_t v = t.base + it->succ->second * val;
            if (update_lower_bound(sn, v)) {
                val = v;
            } else {
                break;
            }
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
            IncumbentSolution new_val(sn.get_value());
            {
                const int i = it->successors.size() - 1;
                const auto& succs = it->successors[i];
                const auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (auto succ = t.successors.begin();
                     succ != t.successors.end();
                     ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    value_utils::add(val, succ->second, succ_info);
                }
                for (auto succ = it->succ; succ != succs.end(); ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    if (succ_info.is_new()) {
                        value_utils::add(val, succ->second, trivial_bound_);
                    } else {
                        value_utils::add(val, succ->second, succ_info);
                    }
                }
                value_utils::mult(val, t.self_loop);
                value_utils::update_incumbent(new_val, val);
            }
            for (int i = it->successors.size() - 2; i >= 0; --i) {
                auto& succs = it->successors[i];
                auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (auto succ = t.successors.begin();
                     succ != t.successors.end();
                     ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    value_utils::add(val, succ->second, succ_info);
                }
                for (auto succ = succs.begin(); succ != succs.end();) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    if (succ_info.is_new()) {
                        value_utils::add(val, succ->second, trivial_bound_);
                        ++succ;
                    } else if (succ_info.is_onstack()) {
                        sn.lowlink = std::min(sn.lowlink, succ_info.lowlink);
                        value_utils::add(val, succ->second, succ_info);
                        t.successors.add(succ->first, succ->second);
                        succ = succs.erase(succ);
                    } else if (
                        succ_info.is_marked_dead_end() && notify_dead_ends_
                        && store_neighbors_) {
                        value_utils::add(val, succ->second, succ_info);
                        ++succ;
                    } else {
                        assert(succ_info.is_closed());
                        t.base += succ_info.get_value() * succ->second;
                        value_utils::add(val, succ->second, succ_info);
                        it->all_successors_are_dead =
                            it->all_successors_are_dead
                            && succ_info.is_dead_end();
                        it->all_successors_marked_dead =
                            it->all_successors_marked_dead
                            && succ_info.is_marked_dead_end();
                        succ = succs.erase(succ);
                    }
                }
                value_utils::mult(val, t.self_loop);
                value_utils::update_incumbent(new_val, val);
            }
            for (unsigned i = st.successors.size() - 1;
                 i > it->successors.size();
                 --i) {
                const auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (auto succ = t.successors.begin();
                     succ != t.successors.end();
                     ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    value_utils::add(val, succ->second, succ_info);
                }
                value_utils::mult(val, t.self_loop);
                value_utils::update_incumbent(new_val, val);
            }
            if (!sn.update_value(true, new_val)
                && only_propagate_when_changed_) {
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
                IncumbentSolution new_val(sn.get_value());
                for (int i = st.successors.size() - 1; i >= 0; --i) {
                    const auto& t = st.successors[i];
                    IncumbentSolution val(t.base);
                    for (auto succ = t.successors.begin();
                         succ != t.successors.end();
                         ++succ) {
                        const SearchNodeInformation& succ_info =
                            search_space_[succ->first];
                        value_utils::add(val, succ->second, succ_info);
                    }
                    value_utils::mult(val, t.self_loop);
                    value_utils::update_incumbent(new_val, val);
                }
                sn.update_value(true, new_val);
            }
            ++statistics_.value_updates;
            --stack_index;
            StackInformation& st = stack_infos_[it->stack_index];
            SearchNodeInformation& sn = search_space_[st.state_ref];
            IncumbentSolution new_val(sn.get_value());
            {
                const int i = it->successors.size() - 1;
                const auto& succs = it->successors[i];
                const auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (auto succ = t.successors.begin();
                     succ != t.successors.end();
                     ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    value_utils::add(val, succ->second, succ_info);
                }
                for (auto succ = it->succ; succ != succs.end(); ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    if (succ_info.is_new()) {
                        value_utils::add(val, succ->second, trivial_bound_);
                    } else {
                        value_utils::add(val, succ->second, succ_info);
                    }
                }
                value_utils::mult(val, t.self_loop);
                value_utils::update_incumbent(new_val, val);
            }
            for (int i = it->successors.size() - 2; i >= 0; --i) {
                auto& succs = it->successors[i];
                auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (auto succ = t.successors.begin();
                     succ != t.successors.end();
                     ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    value_utils::add(val, succ->second, succ_info);
                }
                for (auto succ = succs.begin(); succ != succs.end();) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    if (succ_info.is_new()) {
                        value_utils::add(val, succ->second, trivial_bound_);
                        ++succ;
                    } else if (succ_info.is_onstack()) {
                        sn.lowlink = std::min(sn.lowlink, succ_info.lowlink);
                        value_utils::add(val, succ->second, succ_info);
                        t.successors.add(succ->first, succ->second);
                        succ = succs.erase(succ);
                    } else if (
                        succ_info.is_marked_dead_end() && notify_dead_ends_
                        && store_neighbors_) {
                        value_utils::add(val, succ->second, succ_info);
                        ++succ;
                    } else {
                        assert(succ_info.is_closed());
                        t.base += succ_info.get_value() * succ->second;
                        value_utils::add(val, succ->second, succ_info);
                        it->all_successors_are_dead =
                            it->all_successors_are_dead
                            && succ_info.is_dead_end();
                        it->all_successors_marked_dead =
                            it->all_successors_marked_dead
                            && succ_info.is_marked_dead_end();
                        succ = succs.erase(succ);
                    }
                }
                value_utils::mult(val, t.self_loop);
                value_utils::update_incumbent(new_val, val);
            }
            for (unsigned i = st.successors.size() - 1;
                 i > it->successors.size();
                 --i) {
                const auto& t = st.successors[i];
                IncumbentSolution val(t.base);
                for (auto succ = t.successors.begin();
                     succ != t.successors.end();
                     ++succ) {
                    const SearchNodeInformation& succ_info =
                        search_space_[succ->first];
                    value_utils::add(val, succ->second, succ_info);
                }
                value_utils::mult(val, t.self_loop);
                value_utils::update_incumbent(new_val, val);
            }
            if (!sn.update_value(true, new_val)
                && only_propagate_when_changed_) {
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
            SearchNodeInformation& node = search_space_[stack_info.state_ref];
            node.lowlink = std::min(node.lowlink, lowlink);
            for (int i = exp.successors.size() - 1; i >= 0; --i) {
                const auto& succs = exp.successors[i];
                for (auto it = succs.begin(); it != succs.end(); ++it) {
                    SearchNodeInformation& succ_info = search_space_[it->first];
                    succ_info.set_value(dead_end_value_);
                    if (succ_info.is_onstack()) {
                        node.lowlink =
                            std::min(node.lowlink, succ_info.lowlink);
                    }
                    succ_info.mark_dead_end();
                }
            }
            lowlink = node.lowlink;
            if (exp.stack_index == lowlink) {
                auto it = stack_infos_.rbegin();
                while (true) {
                    SearchNodeInformation& info = search_space_[it->state_ref];
                    info.set_value(dead_end_value_);
                    info.mark_dead_end();
                    if (*(it++) == stack_info.state_ref) {
                        break;
                    }
                }
                if (store_neighbors_) {
                    neighbors_.resize(exp.neighbors_size);
                    neighbors_.push_back(stack_info.state_ref);
                }
                stack_infos_.erase((it.base()), stack_infos_.end());
                expansion_infos_.pop_back();
                break;
            }
            expansion_infos_.pop_back();
        } while (true);
    }

    bool check_early_convergence(
        const std::false_type&,
        const SearchNodeInformation& node) const
    {
        return node.get_value() >= this->get_maximal_reward();
    }

    bool check_early_convergence(
        const std::true_type&,
        const SearchNodeInformation& node) const
    {
        return node.value >= node.value2;
    }

    Statistics statistics_;

    ProgressReport* report_;
    const IncumbentSolution dead_end_value_;
    const IncumbentSolution trivial_bound_;

    StateEvaluator<State>* evaluator_;
    StateEvaluator<State>* dead_end_evaluator_;
    DeadEndListener<State, Action>* dead_end_listener_;
    NewStateHandler<State>* new_state_handler_;

    const BacktrackingUpdateType reverse_path_updates_;
    const bool only_propagate_when_changed_;
    const bool evaluator_recomputation_;
    const bool notify_initial_state_;

    SuccessorSorting<Action>* successor_sort_;

    storage::PerStateStorage<SearchNodeInformation> search_space_;

    std::deque<ExpansionInformation> expansion_infos_;
    std::deque<StackInformation> stack_infos_;
    std::deque<StateID> neighbors_;

    bool last_all_dead_ = true;
    bool last_all_marked_dead_ = true;
    bool backtracked_from_dead_end_scc_ = false;

    bool notify_dead_ends_;
    bool store_neighbors_;
};

} // namespace exhaustive_dfs
} // namespace probabilistic

#undef TIMER_STATISTICS_ENABLED
