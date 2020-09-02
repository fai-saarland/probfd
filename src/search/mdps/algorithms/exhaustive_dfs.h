#pragma once

#include "../../utils/timer.h"
#include "../interfaces/i_engine.h"
#include "../interfaces/i_printable.h"
#include "../progress_report.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"
#include "value_utils.h"

#include <cassert>
#include <deque>
#include <memory>

namespace probabilistic {
namespace algorithms {
namespace exhaustive_dfs {

enum class PathUpdates {
    Disabled,
    Changes,
    Complete,
};

namespace internal {
class Statistics : public IPrintable {
public:
    Statistics()
    {
        notification_time.stop();
        evaluation_time.stop();
    }

    virtual void print(std::ostream& out) const override
    {
        out << "Expanded " << expanded << " state(s)." << std::endl;
        out << "Evaluated " << evaluated << " state(s)." << std::endl;
        out << "Evaluations: " << evaluations << std::endl;
        out << "Goal states: " << goal_states << " state(s)." << std::endl;
        out << "Dead ends: " << dead_ends << " state(s)." << std::endl;
        out << "Transitions: " << transitions << std::endl;
        out << "Generated " << generated_states << " state(s)." << std::endl;
        out << "Generated " << generated_transitions << " transition(s)."
            << std::endl;
        out << "State value updates: " << value_updates << std::endl;
        out << "Path updates: " << path_updates << std::endl;
        out << "Backtracked from " << backtracks << " state(s)." << std::endl;
        out << "Found " << sccs << " SCC(s)." << std::endl;
        out << "Found " << dead_end_sccs << " dead-end SCC(s)." << std::endl;
        out << "Partially pruned " << pruned_dead_end_sccs
            << " dead-end SCC(s)." << std::endl;
        out << "Dead end evaluator time: " << evaluation_time << std::endl;
        out << "Dead end notifications: " << notifications << std::endl;
        out << "Total dead end nofication time: " << (notification_time)
            << std::endl;
        out << "Average dead-end SCC size: "
            << (static_cast<double>(summed_dead_end_scc_sizes)
                / static_cast<int>(dead_end_sccs))
            << std::endl;
        out << "Average dead-end SCC size for notifications: "
            << (static_cast<double>(summed_notified_scc_sizes)
                / static_cast<int>(notifications))
            << std::endl;
    }

    void evaluation_started()
    {
        evaluations++;
        evaluation_time.resume();
    }

    void evaluation_finished() { evaluation_time.stop(); }

    void notification_started(unsigned scc_size)
    {
        notifications++;
        notification_time.resume();
        summed_notified_scc_sizes += scc_size;
    }

    void notification_finished() { notification_time.stop(); }

    unsigned long long expanded = 0;
    unsigned long long evaluations = 0;
    unsigned long long evaluated = 0;
    unsigned long long goal_states = 0;
    unsigned long long transitions = 0;
    unsigned long long generated_states = 0;
    unsigned long long generated_transitions = 0;
    unsigned long long dead_ends = 0;

    unsigned long long value_updates = 0;
    unsigned long long path_updates = 0;
    unsigned long long backtracks = 0;
    unsigned long long sccs = 0;
    unsigned long long dead_end_sccs = 0;
    unsigned long long pruned_dead_end_sccs = 0;
    unsigned long long notifications = 0;
    unsigned long long summed_dead_end_scc_sizes = 0;
    unsigned long long summed_notified_scc_sizes = 0;

    ::utils::Timer evaluation_time;
    ::utils::Timer notification_time;
};

struct SearchNodeCoreInformation {
    static const uint8_t NEW = 0;
    static const uint8_t CLOSED = 1;
    static const uint8_t OPEN = 2;
    static const uint8_t ONSTACK = 4;
    static const uint8_t DEAD = 3;
    static const uint8_t MARKED = 7;

    SearchNodeCoreInformation()
        : status(NEW)
        , index(-1)
        , lowlink(-1)
        , value(value_type::zero)
    {
    }

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
        index = lowlink = idx;
    }

    void update_lowlink(const unsigned idx)
    {
        assert(is_onstack());
        lowlink = std::min(lowlink, idx);
    }

    void close() { status = CLOSED; }

    void set_dead_end()
    {
        assert(!is_marked_dead_end());
        status = DEAD;
    }

    void mark_dead_end() { status = MARKED; }

    uint8_t status;
    unsigned index;
    unsigned lowlink;
    value_type::value_t value;
};

template<typename Dual = std::false_type>
struct SearchNodeInformation : public SearchNodeCoreInformation {
    bool update_value(const value_utils::IncumbentSolution<Dual>& val)
    {
        const bool result = value_type::approx_equal()(val.first, value);
        value = val.first;
        return !result;
    }

    void set_estimate(const EvaluationResult&) { }
};

template<>
struct SearchNodeInformation<std::true_type>
    : public SearchNodeCoreInformation {

    SearchNodeInformation()
        : SearchNodeCoreInformation()
        , value2(value_type::zero)
    {
    }

    bool
    update_value(const value_utils::IncumbentSolution<std::false_type>& val)
    {
        const bool result = value_type::approx_equal()(val.first, value);
        value = val.first;
        value2 = val.first;
        return !result;
    }

    bool update_value(const value_utils::IncumbentSolution<std::true_type>& val)
    {
        const bool result = value_type::approx_equal()(val.first, value)
            && value_type::approx_equal()(val.second, value2);
        value = val.first;
        value2 = val.second;
        return !result;
    }

    void set_estimate(const EvaluationResult& est)
    {
        value2 = (value_type::value_t)est;
    }

    value_type::value_t value2;
};

} // namespace internal

template<typename State, typename Action, typename DualV>
class ExhaustiveDepthFirstSearch : public IMDPEngine<State> {
public:
    using DualBounds = DualV;
    using SearchNodeInformation =
        typename internal::SearchNodeInformation<DualBounds>;

    explicit ExhaustiveDepthFirstSearch(
        StateEvaluationFunction<State>* evaluator,
        StateEvaluationFunction<State>* dead_end_eval,
        StateComponentListener<State>* dead_end_listener,
        bool reevaluate,
        bool notify_initial,
        OpenList<State, Action>* open_list,
        PathUpdates path_updates,
        ProgressReport* progress,
        StateIDMap<State>* id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        NewStateHandler<State, Action>* new_state_handler,
        value_type::value_t dead_end_value)
        : statistics_()
        , report_(progress)
        , state_id_map_(id_map)
        , aops_generator_(aops_gen)
        , transition_generator_(transition_gen)
        , state_reward_(state_reward)
        , transition_reward_(transition_reward)
        , dead_end_value_(dead_end_value)
        , open_list_(open_list)
        , evaluator_(evaluator)
        , dead_end_evaluator_(dead_end_eval)
        , dead_end_listener_(dead_end_listener)
        , new_state_handler_(new_state_handler)
        , reverse_path_updates_(path_updates)
        , evaluator_recomputation_(reevaluate)
        , notify_initial_state_(notify_initial)
        , search_space_(SearchNodeInformation(), state_id_map_)
        , current_index_(0)
        , backtracked_(false)
        , backtracked_all_dead_(false)
        , backtracked_all_marked_dead_(false)
        , backtracked_from_(nullptr)
        , notify_dead_ends_(dead_end_listener != nullptr)
        , store_neighbors_(
              dead_end_listener_ != nullptr
              && dead_end_listener_->neighbor_dependency())
    {
    }

    virtual AnalysisResult solve(const State& state) override
    {
        StateID stateid = this->state_id_map_->operator[](state);
        SearchNodeInformation& info = search_space_[stateid];
        backtracked_from_ = &info;
        if (!initialize_search_node(state, info)) {
            std::cout << "initial state is goal state!" << std::endl;
        } else if (!push_state(stateid)) {
            std::cout << "initial state is dead end!" << std::endl;
        } else {
            register_value_reports(DualBounds(), &info);
            while (!expansion_infos_.empty()) {
                step();
            }
            backtracked_from_ = &info;
        }
        return AnalysisResult(
            backtracked_from_->value, new internal::Statistics(statistics_));
    }

private:
    struct ExpansionInformation {

        explicit ExpansionInformation(
            const StateID& state_ref,
            const State& state,
            value_type::value_t state_reward,
            unsigned queue_size,
            unsigned neighbors_size)
            : state_ref(state_ref)
            , state(state)
            , state_reward(state_reward)
            , open_queue_size(queue_size)
            , neighbors_size(neighbors_size)
            , all_successors_are_dead(true)
            , all_successors_marked_dead(true)
        {
        }

        StateID state_ref;
        State state;
        value_type::value_t state_reward;
        unsigned open_queue_size;
        unsigned neighbors_size;
        bool all_successors_are_dead;
        bool all_successors_marked_dead;
    };

    struct StackInformation {
        explicit StackInformation(
            const StateID& state_ref,
            const value_type::value_t& state_reward)
            : state_ref(state_ref)
            , state_reward(state_reward)
        {
        }

        bool operator==(const StateID& state) const
        {
            return state_ref == state;
        }

        StateID state_ref;
        value_type::value_t state_reward;
    };

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
        statistics_.evaluation_started();
        const EvaluationResult res = evaluator_->operator()(state);
        statistics_.evaluation_finished();
        return res;
    }

    bool is_dead_end(const State& state)
    {
        statistics_.evaluation_started();
        bool result = false;
        if (dead_end_evaluator_ != nullptr) {
            result = (bool)dead_end_evaluator_->operator()(state);
        }
        statistics_.evaluation_finished();
        return result;
    }

    void mark_current_component_dead()
    {
        statistics_.sccs++;
        statistics_.dead_end_sccs++;
        statistics_.pruned_dead_end_sccs++;
        assert(!expansion_infos_.empty());
        unsigned lowlink = -1;
        while (true) {
            ExpansionInformation& exp = expansion_infos_.back();
            SearchNodeInformation& node = search_space_[exp.state_ref];
            node.update_lowlink(lowlink);
            while (open_queue_.size() > exp.open_queue_size) {
                StateID succ = open_queue_.back();
                open_queue_.pop_back();
                SearchNodeInformation& succ_info = search_space_[succ];
                succ_info.update_value(dead_end_value_);
                if (succ_info.is_onstack()) {
                    node.update_lowlink(succ_info.index);
                }
                succ_info.mark_dead_end();
            }
            lowlink = node.lowlink;
            if (node.index == node.lowlink) {
                auto it = stack_infos_.rbegin();
                while (true) {
                    SearchNodeInformation& info = search_space_[it->state_ref];
                    info.update_value(dead_end_value_);
                    info.mark_dead_end();
                    if (*(it++) == exp.state_ref) {
                        break;
                    }
                }
                stack_infos_.erase((it.base()), stack_infos_.end());
                if (store_neighbors_) {
                    neighbors_.resize(exp.neighbors_size);
                    neighbors_.push_back(exp.state_ref);
                }
                expansion_infos_.pop_back();
                break;
            }
            expansion_infos_.pop_back();
        }
    }

    bool initialize_search_node(const State& state, SearchNodeInformation& info)
    {
        assert(info.is_new());
        auto reward = state_reward_->operator()(state);
        if (reward) {
            info.close();
            info.update_value(value_utils::IncumbentSolution<DualBounds>(
                (value_type::value_t)reward));
            ++statistics_.goal_states;
            if (new_state_handler_) {
                new_state_handler_->touch_goal(state);
            }
            return false;
        } else {
            reward = evaluate(state);
            info.update_value(dead_end_value_);
            if (reward) {
                info.mark_dead_end();
                ++statistics_.dead_ends;
                if (new_state_handler_) {
                    new_state_handler_->touch_dead_end(state);
                }
                return false;
            } else {
                info.set_estimate(reward);
                info.open();
                if (new_state_handler_) {
                    new_state_handler_->touch(state);
                }
            }
        }
        return true;
    }

    template<typename T>
    value_utils::IncumbentSolution<T> dead_end_value() const
    {
        return value_utils::IncumbentSolution<T>(dead_end_value_.first);
    }

    template<typename Dual = DualBounds>
    bool value_update(
        const StateID& state_id,
        const State& state,
        const value_type::value_t& reward)
    {
        statistics_.value_updates++;
        SearchNodeInformation& info = search_space_[state_id];
        std::vector<Action> aops;
        aops_generator_->operator()(state, aops);
        value_utils::IncumbentSolution<Dual> newval = dead_end_value<Dual>();
        statistics_.generated_transitions += aops.size();
        for (int i = aops.size() - 1; i >= 0; --i) {
            Distribution<State> successors =
                transition_generator_->operator()(state, aops[i]);
            statistics_.generated_states += successors.size();
            value_utils::IncumbentSolution<Dual> tval(
                reward + transition_reward_->operator()(state, aops[i]));
            value_type::value_t self_prob = value_type::zero;
            bool has_self_loop = false;
            bool pure_self_loops = true;
            for (auto it = successors.begin(); it != successors.end(); ++it) {
                assert(!search_space_[it->first].is_new());
                if (state_id_map_->operator[](it->first) == state_id) {
                    self_prob += it->second;
                    has_self_loop = true;
                } else {
                    pure_self_loops = false;
                    value_utils::add(
                        tval, it->second, search_space_[it->first]);
                }
            }
            if (!pure_self_loops) {
                if (has_self_loop) {
                    value_utils::mult(
                        tval, value_type::one / (value_type::one - self_prob));
                }
                value_utils::update_incumbent(newval, tval);
            }
        }
        return info.update_value(newval);
    }

    void update_state_values_on_path(
        typename std::deque<ExpansionInformation>::const_reverse_iterator it)
    {
        statistics_.path_updates++;
        bool updated = true;
        for (; it != expansion_infos_.rend() && updated; ++it) {
            updated = value_update(it->state_ref, it->state, it->state_reward)
                || reverse_path_updates_ == PathUpdates::Complete;
        }
        if (it == expansion_infos_.rend()) {
            // const internal::SearchNodeInformation& iinfo =
            //     search_space_[expansion_infos_.front().state_ref];
            // early_termination_condition_ =
            //     equal_(iinfo.value, property_->get_optimistic_bound());
            this->report_->operator()();
        }
    }

    bool push_state(const StateID& state_ref)
    {
        State state = this->state_id_map_->operator[](state_ref);
        SearchNodeInformation& info = search_space_[state_ref];
        assert(info.is_open());

        statistics_.evaluated++;
        if (is_dead_end(state)) {
            statistics_.dead_ends++;
            info.update_value(dead_end_value_);
            info.mark_dead_end();
            return false;
        }

        statistics_.expanded++;

        info.set_onstack(stack_infos_.size());
        stack_infos_.emplace_back(state_ref, info.value);
        expansion_infos_.emplace_back(
            state_ref,
            state,
            info.value,
            open_queue_.size(),
            neighbors_.size());
        ExpansionInformation& exp = expansion_infos_.back();

        std::vector<Action> aops;
        aops_generator_->operator()(state, aops);
        statistics_.generated_transitions += aops.size();
        statistics_.transitions += aops.size();
        bool update_path = false;
        for (unsigned i = 0; i < aops.size(); ++i) {
            Distribution<State> successors =
                transition_generator_->operator()(state, aops[i]);
            statistics_.generated_states += successors.size();
            for (auto it = successors.begin(); it != successors.end(); ++it) {
                StateID succid = this->state_id_map_->operator[](it->first);
                SearchNodeInformation& succ_info = search_space_[succid];
                if (succ_info.is_new()) {
                    initialize_search_node(it->first, succ_info);
                }
                if (succ_info.is_open()) {
                    open_list_->push(
                        succid, state, aops[i], it->second, it->first);
                } else if (succ_info.is_onstack()) {
                    info.update_lowlink(succ_info.index);
                } else if (!succ_info.is_dead_end()) {
                    update_path = true;
                    exp.all_successors_are_dead = false;
                    exp.all_successors_marked_dead = false;
                } else if (!succ_info.is_marked_dead_end()) {
                    update_path = update_path || DualBounds::value;
                    exp.all_successors_marked_dead = false;
                } else if (store_neighbors_ && exp.all_successors_marked_dead) {
                    update_path = update_path || DualBounds::value;
                    neighbors_.push_back(succid);
                }
            }
        }

        auto it = open_queue_.end();
        while (!open_list_->empty()) {
            it = open_queue_.insert(it, open_list_->pop());
        }

        if (reverse_path_updates_ != PathUpdates::Disabled && update_path) {
            update_state_values_on_path(expansion_infos_.rbegin());
        }

        return true;
    }

    void step()
    {
        assert(!expansion_infos_.empty());
        assert(!stack_infos_.empty());
        ExpansionInformation& expanding = expansion_infos_.back();
        SearchNodeInformation& node_info = search_space_[expanding.state_ref];
        if (backtracked_) {
            if (backtracked_from_->is_onstack()) {
                node_info.update_lowlink(backtracked_from_->lowlink);
                expanding.all_successors_are_dead &= backtracked_all_dead_;
                expanding.all_successors_marked_dead &=
                    backtracked_all_marked_dead_;
            } else if (backtracked_all_marked_dead_) {
                if (evaluator_recomputation_ && is_dead_end(expanding.state)) {
                    mark_current_component_dead();
                    backtracked_from_ = &node_info;
                    return;
                }
            } else if (backtracked_all_dead_) {
                expanding.all_successors_marked_dead = false;
            } else {
                expanding.all_successors_are_dead = false;
                expanding.all_successors_marked_dead = false;
            }
        }
        backtracked_ = true;
        bool skipped = false;
        while (open_queue_.size() > expanding.open_queue_size) {
            StateID successor = open_queue_.back();
            open_queue_.pop_back();
            SearchNodeInformation& successor_info = search_space_[successor];
            assert(!successor_info.is_new());
            if (successor_info.is_open()) {
                if (push_state(successor)) {
                    backtracked_ = false;
                    break;
                } else if (
                    store_neighbors_ && expanding.all_successors_marked_dead) {
                    assert(successor_info.is_marked_dead_end());
                    neighbors_.push_back(successor);
                }
#if !defined(NDEBUG)
                else {
                    assert(successor_info.is_marked_dead_end());
                }
#endif
                skipped = true;
            } else if (successor_info.is_onstack()) {
                node_info.update_lowlink(successor_info.index);
            } else {
                assert(successor_info.is_closed());
                if (!successor_info.is_dead_end()) {
                    expanding.all_successors_are_dead = false;
                    expanding.all_successors_marked_dead = false;
                } else if (!successor_info.is_marked_dead_end()) {
                    expanding.all_successors_marked_dead = false;
                } else if (
                    store_neighbors_ && expanding.all_successors_marked_dead) {
                    neighbors_.push_back(successor);
                }
            }
        }
        if (backtracked_) {
            statistics_.backtracks++;
            assert(expanding.open_queue_size == open_queue_.size());
            backtracked_from_ = &node_info;
            backtracked_all_dead_ = expanding.all_successors_are_dead;
            backtracked_all_marked_dead_ = expanding.all_successors_marked_dead;
            if (node_info.index == node_info.lowlink) {
                ++statistics_.sccs;
                bool update_value = false;
                auto rend = stack_infos_.rbegin();
                if (expanding.all_successors_are_dead) {
                    unsigned scc_size = 0;
                    while (true) {
                        scc_size++;
                        SearchNodeInformation& info =
                            search_space_[rend->state_ref];
                        info.update_value(dead_end_value_);
                        info.set_dead_end();
                        if ((rend++)->state_ref == expanding.state_ref) {
                            break;
                        }
                    }
                    statistics_.dead_end_sccs++;
                    statistics_.summed_dead_end_scc_sizes += scc_size;
                    if (notify_dead_ends_
                        && (expanding.all_successors_marked_dead
                            || !dead_end_listener_->neighbor_dependency())
                        && (notify_initial_state_
                            || expansion_infos_.size() > 1)) {
                        statistics_.notification_started(scc_size);

                        std::deque<State> component;
                        for (auto sit = stack_infos_.rbegin(); sit != rend;
                             ++sit) {
                            component.push_back(
                                state_id_map_->operator[](sit->state_ref));
                        }

                        storage::StateHashSet<State> neighbors;
                        if (store_neighbors_) {
                            auto nit = neighbors_.rbegin();
                            for (int i = neighbors_.size()
                                     - expanding.neighbors_size;
                                 i > 0;
                                 --i, ++nit) {
                                neighbors.insert(
                                    state_id_map_->operator[](*nit));
                            }
                        }

                        if (dead_end_listener_->operator()(
                                component.begin(),
                                component.end(),
                                neighbors.begin(),
                                neighbors.end())) {
                            for (auto it = stack_infos_.rbegin(); it != rend;
                                 ++it) {
                                search_space_[it->state_ref].mark_dead_end();
                            }
                        } else {
                            backtracked_all_marked_dead_ = false;
                            notify_dead_ends_ = false;
                            store_neighbors_ = false;
                            std::deque<StateID>().swap(neighbors_);
                        }

                        statistics_.notification_finished();
                    } else {
                        backtracked_all_marked_dead_ = false;
                    }
                    update_value = DualBounds::value;
                } else {
                    std::vector<State> component;
                    do {
                        SearchNodeInformation& info =
                            search_space_[rend->state_ref];
                        info.close();
                        component.emplace_back(
                            state_id_map_->operator[](rend->state_ref));
                        if (rend->state_ref == expanding.state_ref) {
                            ++rend;
                            break;
                        }
                        ++rend;
                    } while (true);

                    unsigned iterations = 0;
                    bool changed = true;
                    while (changed) {
                        changed = false;
                        const State* s = &component[0];
                        for (auto it = stack_infos_.rbegin(); it != rend;
                             ++it, ++s) {
                            changed = value_update<std::false_type>(
                                          it->state_ref, *s, it->state_reward)
                                || changed;
                        }
                        ++iterations;
                    }

                    update_value = iterations > 1;
                }

                stack_infos_.erase((rend.base()), stack_infos_.end());
                if (store_neighbors_) {
                    neighbors_.resize(expanding.neighbors_size);
                    if (node_info.is_marked_dead_end()) {
                        neighbors_.push_back(expanding.state_ref);
                    }
                }

                if (reverse_path_updates_ != PathUpdates::Disabled
                    && update_value) {
                    update_state_values_on_path(++expansion_infos_.rbegin());
                }
            }
            expansion_infos_.pop_back();
        } else if (
            DualBounds::value && skipped
            && reverse_path_updates_ != PathUpdates::Disabled) {
            update_state_values_on_path(++expansion_infos_.rbegin());
        }
    }

    internal::Statistics statistics_;

    ProgressReport* report_;
    StateIDMap<State>* state_id_map_;
    ApplicableActionsGenerator<State, Action>* aops_generator_;
    TransitionGenerator<State, Action>* transition_generator_;
    StateEvaluationFunction<State>* state_reward_;
    TransitionRewardFunction<State, Action>* transition_reward_;
    const value_utils::IncumbentSolution<DualBounds> dead_end_value_;
    OpenList<State, Action>* open_list_;
    StateEvaluationFunction<State>* evaluator_;
    StateEvaluationFunction<State>* dead_end_evaluator_;
    StateComponentListener<State>* dead_end_listener_;
    NewStateHandler<State, Action>* new_state_handler_;
    const PathUpdates reverse_path_updates_;
    const bool evaluator_recomputation_;
    const bool notify_initial_state_;

    storage::PerStateStorage<SearchNodeInformation, StateIDMap<State>>
        search_space_;

    unsigned current_index_;
    std::deque<ExpansionInformation> expansion_infos_;
    std::deque<StackInformation> stack_infos_;
    std::deque<StateID> open_queue_;
    std::deque<StateID> neighbors_;

    bool backtracked_;
    bool backtracked_all_dead_;
    bool backtracked_all_marked_dead_;
    SearchNodeInformation* backtracked_from_;

    bool notify_dead_ends_;
    bool store_neighbors_;
};

} // namespace exhaustive_dfs
} // namespace algorithms
} // namespace probabilistic
