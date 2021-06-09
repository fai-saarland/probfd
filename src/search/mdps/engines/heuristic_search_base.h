#pragma once

#include "../engine_interfaces/dead_end_listener.h"
#include "../engine_interfaces/heuristic_search_connector.h"
#include "../engine_interfaces/new_state_handler.h"
#include "../engine_interfaces/policy_picker.h"
#include "../progress_report.h"
#include "../storage/per_state_storage.h"
#include "../utils/graph_visualization.h"
#include "engine.h"
#include "heuristic_search_state_information.h"
#include "../value_utils.h"

#if defined(EXPENSIVE_STATISTICS)
#include "../../utils/timer.h"
#endif

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace engines {
namespace heuristic_search {

namespace internal {

struct CoreStatistics {
    unsigned long long backups = 0;

    unsigned long long backed_up_states = 0;
    unsigned long long evaluated_states = 0;
    unsigned long long pruned_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long self_loop_states = 0;
    unsigned long long value_changes = 0;
    unsigned long long policy_updates = 0;
};

struct Statistics : public CoreStatistics {

    unsigned state_info_bytes = 0;
    value_type::value_t initial_state_estimate = 0;
    bool initial_state_found_terminal = 0;

    unsigned long long externally_set_dead_ends = 0;
    unsigned long long dead_end_safe_updates = 0;
    unsigned long long dead_end_safe_updates_states = 0;
    unsigned long long dead_end_safe_updates_dead_ends = 0;

    unsigned long long wrongly_classified_dead_ends = 0;
    unsigned long long safe_updates_non_dead_end_value = 0;

    value_type::value_t value = value_type::zero;
    CoreStatistics before_last_update;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer update_time;
    utils::Timer policy_selection_time;
#endif

    Statistics()
    {
#if defined(EXPENSIVE_STATISTICS)
        update_time.stop();
        policy_selection_time.stop();
#endif
    }

    void print(std::ostream& out) const
    {
        out << "  Stored " << state_info_bytes << " bytes per state"
            << std::endl;

        out << "  Initial state value estimation: " << initial_state_estimate
            << std::endl;
        out << "  Initial state value found terminal: "
            << initial_state_found_terminal << std::endl;

        out << "  Evaluated state(s): " << evaluated_states << std::endl;
        out << "  Pruned state(s): " << pruned_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Self-loop state(s): " << self_loop_states << std::endl;
        out << "  Backed up state(s): " << backed_up_states << std::endl;
        out << "  Number of backups: " << backups << std::endl;
        out << "  Number of value changes: " << value_changes << std::endl;
        out << "  Number of policy updates: " << policy_updates << std::endl;

        out << "  Evaluated state(s) until last value change: "
            << before_last_update.evaluated_states << std::endl;
        out << "  Pruned state(s) until last value change: "
            << before_last_update.pruned_states << std::endl;
        out << "  Goal state(s) until last value change: "
            << before_last_update.goal_states << std::endl;
        out << "  Terminal state(s) until last value change: "
            << before_last_update.terminal_states << std::endl;
        out << "  Self-loop state(s) until last value change: "
            << before_last_update.self_loop_states << std::endl;
        out << "  Backed up state(s) until last value change: "
            << before_last_update.backed_up_states << std::endl;
        out << "  Number of backups until last value change: "
            << before_last_update.backups << std::endl;
        out << "  Number of value changes until last value change: "
            << before_last_update.value_changes << std::endl;
        out << "  Number of policy updates until last value change: "
            << before_last_update.policy_updates << std::endl;

#if defined(EXPENSIVE_STATISTICS)
        out << "  Updating time: " << update_time << std::endl;
        out << "  Policy selection time: " << policy_selection_time
            << std::endl;
#endif

        out << "  Number of dead-end identification runs: "
            << dead_end_safe_updates << std::endl;
        out << "  Dead-end identification runs on states with non dead-end "
               "value: "
            << safe_updates_non_dead_end_value << std::endl;
        out << "  States considered while checking dead-end property: "
            << dead_end_safe_updates_states << std::endl;
        out << "  Wrongly classified dead-ends: "
            << wrongly_classified_dead_ends << std::endl;
        out << "  Dead-ends identified: " << dead_end_safe_updates_dead_ends
            << std::endl;
        out << "  Externally set dead-ends: " << externally_set_dead_ends
            << std::endl
            << std::flush;
    }

    void jump() { before_last_update = *this; }
};

template<typename StateT, typename ActionT, typename StateInfoT>
class HeuristicSearchBase : public MDPEngine<StateT, ActionT>,
                            public PerStateInformationLookup {
    template<typename T>
    friend class StateStatusAccessor;

public:
    using State = StateT;
    using Action = ActionT;
    using StateInfo = StateInfoT;
    using StorePolicy = typename StateInfo::StoresPolicy;
    using DualBounds = typename StateInfo::DualBounds;

    using IncumbentSolution = value_utils::IncumbentSolution<DualBounds>;

    template<typename T>
    class StateStatusAccessor {
    public:
        StateInfo& operator()(const StateID& state_id)
        {
            return infos_->state_infos_[state_id];
        }

        StateInfo& operator()(const StateID id, T& info)
        {
            return this->operator()(std::is_same<T, StateInfo>(), id, info);
        }

    private:
        friend class HeuristicSearchBase;

        StateStatusAccessor(HeuristicSearchBase* infos)
            : infos_(infos)
        {
        }

        StateInfo& operator()(const std::true_type&, const StateID, T& info)
        {
            return info;
        }

        StateInfo& operator()(const std::false_type&, const StateID id, T&)
        {
            return infos_->state_infos_[id];
        }

        HeuristicSearchBase* infos_;
    };

    explicit HeuristicSearchBase(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        DeadEndIdentificationLevel,
        StateEvaluator<State>* dead_end_eval,
        DeadEndListener<State, Action>* dead_end_listener,
        PolicyPicker<Action>* policy_chooser,
        NewStateHandler<State>* new_state_handler,
        StateEvaluator<State>* value_init,
        HeuristicSearchConnector* connector,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy)
        : MDPEngine<StateT, ActionT>(
            state_id_map,
            action_id_map,
            state_reward_function,
            action_reward_function,
            minimal_reward,
            maximal_reward,
            aops_generator,
            transition_generator)
        , report_(report)
        , interval_comparison_(interval_comparison)
        , stable_policy_(stable_policy)
        , value_initializer_(value_init)
        , policy_chooser_(policy_chooser)
        , on_new_state_(new_state_handler)
        , dead_end_listener_(dead_end_listener)
        , dead_end_eval_(dead_end_eval)
        , dead_end_value_(this->get_minimal_reward())
        , initial_state_id_(StateID::undefined)
    {
        statistics_.state_info_bytes = sizeof(StateInfo);
        connector->set_lookup_function(this);
    }

    virtual ~HeuristicSearchBase() = default;

    virtual value_type::value_t get_result(const State& s) override
    {
        const StateInfo& info = state_infos_[this->get_state_id(s)];
        return value_utils::as_upper_bound(info.value);
    }

    virtual bool supports_error_bound() const override
    {
        return DualBounds::value;
    }

    virtual value_type::value_t get_error(const State& s) override
    {
        if constexpr (DualBounds::value) {
            const StateInfo& info = state_infos_[this->get_state_id(s)];
            return info.value.error_bound();
        } else {
            return std::numeric_limits<value_type::value_t>::infinity();
        }
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    virtual const void* lookup(const StateID& state_id) override
    {
        return &state_infos_[state_id];
    }

    template<typename T = StateInfo>
    StateStatusAccessor<T> get_state_status_access()
    {
        return StateStatusAccessor<T>(this);
    }

    template<typename T>
    storage::PerStateStorage<T>*
    create_per_state_store(const T& default_value = T())
    {
        return new storage::PerStateStorage<T>(
            default_value, this->state_id_map_);
    }

    bool is_terminal(const StateID& state_id)
    {
        return state_infos_[state_id].is_terminal();
    }

    value_type::value_t get_value(const StateID& state_id)
    {
        return state_infos_[state_id].get_value();
    }

    bool has_dead_end_value(const StateID& state)
    {
        const StateInfo& info = state_infos_[state];
        return info.value == dead_end_value_;
    }

    bool is_marked_dead_end(const StateID& state)
    {
        const StateInfo& info = state_infos_[state];
        return info.is_dead_end();
    }

    void clear_policy(const StateID& state_id)
    {
        static_assert(StorePolicy::value, "Policy not stored by algorithm!");
        
        state_infos_[state_id].set_policy(ActionID::undefined);
    }

    Action get_policy(const StateID& state_id)
    {
        static_assert(StorePolicy::value, "Policy not stored by algorithm!");

        const ActionID aid = state_infos_[state_id].get_policy();
        assert(aid != ActionID::undefined);
        return this->lookup_action(state_id, aid);
    }

    bool apply_policy(const StateID& state, Distribution<StateID>& result)
    {
        static_assert(StorePolicy::value, "Policy not stored by algorithm!");

        const StateInfo& info = state_infos_[state];
        if (info.policy == ActionID::undefined) {
            return async_update(state, nullptr, &result);
        } else {
            Action action = this->lookup_action(state, info.policy);
            this->generate_successors(state, action, result);
            return false;
        }
    }

    void set_dead_end(const StateID& state_id)
    {
        StateInfo& info = state_infos_[state_id];
        info.set_recognized_dead_end();
        info.value = dead_end_value_;
    }

    bool conditional_set_dead_end(const StateID& state_id)
    {
        StateInfo& info = state_infos_[state_id];
        if (!info.is_goal_state()) {
            info.set_recognized_dead_end();
            info.set_value(dead_end_value_);
            return true;
        }
        return false;
    }

    void set_dead_end(StateInfo& info)
    {
        info.set_recognized_dead_end();
        info.value = dead_end_value_;
    }

    void notify_dead_end(const StateID& state_id)
    {
        if (dead_end_listener_ != nullptr) {
            notify_dead_end(state_id, state_infos_[state_id]);
        }
    }

    void notify_dead_end(const StateID& state_id, StateInfo& state_info)
    {
        if (dead_end_listener_ != nullptr
            && !state_info.is_recognized_dead_end()) {
            set_dead_end(state_info);
            dead_end_listener_->operator()(state_id);
        }
    }

    bool conditional_notify_dead_end(const StateID& state_id)
    {
        return conditional_notify_dead_end(state_id, state_infos_[state_id]);
    }

    bool
    conditional_notify_dead_end(const StateID& state_id, StateInfo& state_info)
    {
        if (state_info.is_goal_state()) {
            return false;
        }
        if (dead_end_listener_ != nullptr
            && !state_info.is_recognized_dead_end()) {
            set_dead_end(state_info);
            dead_end_listener_->operator()(state_id);
        }
        return true;
    }

    bool check_dead_end(const StateID& state_id)
    {
        if (dead_end_eval_ != nullptr) {
            State state = this->lookup_state(state_id);
            if (dead_end_eval_->operator()(state)) {
                set_dead_end(state_id);
                return true;
            }
        }
        return false;
    }

    bool is_dead_end_learning_enabled() const
    {
        return dead_end_listener_ != nullptr;
    }

    bool async_update(
        const StateID& s,
        ActionID* policy_action = nullptr,
        Distribution<StateID>* policy_transition = nullptr,
        bool* policy_changed = nullptr)
    {
        return async_update(
            stable_policy_,
            s,
            this->policy_chooser_,
            policy_action,
            policy_transition,
            policy_changed);
    }

    template<typename T>
    bool async_update(
        const StateID& s,
        T* policy_tiebreaker,
        ActionID* policy_action,
        Distribution<StateID>* policy_transition,
        bool* policy_changed)
    {
        return async_update(
            false,
            s,
            policy_tiebreaker,
            policy_action,
            policy_transition,
            policy_changed);
    }

    template<typename AlternativeCondition>
    std::pair<bool, bool>
    safe_async_update(const StateID& s, AlternativeCondition& alt_cond)
    {
        return this->safe_async_update(s, *dead_end_listener_, alt_cond);
    }

    template<typename DeadEndListener, typename AlternativeCondition>
    std::pair<bool, bool> safe_async_update(
        const StateID& s,
        DeadEndListener& listener,
        AlternativeCondition& alt_cond)
    {
        // std::cout << "safe_async_update(" << state_id_map_->operator[](s) <<
        // ")"
        //           << std::endl;

        struct ExpansionData {
            ExpansionData(const StateID& s, int pidx)
                : state(s)
                , pidx(pidx)
                , root(true)
                , lowlink(-1)
            {
            }
            ExpansionData(ExpansionData&& other) = default;
            void update_lowlink(int lk)
            {
                if (lk < lowlink) {
                    lowlink = lk;
                    root = false;
                }
            }
            StateID state;
            std::vector<Action> aops;
            unsigned pidx;
            bool root;
            int lowlink;
        };

        struct PersistentStateData {
            PersistentStateData()
                : onstack(false)
                , index(-1)
            {
            }
            bool onstack;
            int index;
        };

        statistics_.safe_updates_non_dead_end_value += !has_dead_end_value(s);
        statistics_.dead_end_safe_updates++;
        std::pair<bool, bool> result = std::pair<bool, bool>(false, false);

        int index = 0;
        storage::StateHashMap<PersistentStateData> indices;
        std::deque<ExpansionData> expansion_queue;
        std::deque<StateID> stack;
        expansion_queue.emplace_back(s, 0);
        while (!expansion_queue.empty()) {
            assert(!result.first);
            auto& expansion_data = expansion_queue.back();
            const StateID state_id = expansion_data.state;
            auto& state_info = state_infos_[state_id];
            // std::cout << state_id << " -> I"
            //           << state_info.is_value_initialized() << "|G"
            //           << state_info.is_goal_state() << "|D"
            //           << state_info.is_dead_end() << "|R"
            //           << state_info.is_recognized_dead_end() << "|T"
            //           << state_info.is_terminal() << std::endl;
            if (state_info.is_recognized_dead_end()) {
                expansion_queue.pop_back();
            } else if (state_info.is_dead_end()) {
                statistics_.dead_end_safe_updates_states++;
                statistics_.dead_end_safe_updates_dead_ends++;
                state_info.value = dead_end_value_;
                state_info.set_recognized_dead_end();
                listener(expansion_data.state);
                expansion_queue.pop_back();
            } else if (state_info.is_goal_state()) {
                result.first = true;
                const unsigned size = expansion_data.pidx;
                while (expansion_queue.size() != size)
                    expansion_queue.pop_back();
                break;
            } else {
                // std::cout
                //     << "Top() = " << expansion_data.state << " ("
                //     <<
                //     (state_infos_->operator[](expansion_data.state).get_value())
                //     << ")" << std::endl;
                if (expansion_data.aops.empty()) {
                    statistics_.dead_end_safe_updates_states++;
                    if (alt_cond(expansion_data.state)) {
                        result.first = true;
                        const unsigned size = expansion_data.pidx;
                        while (expansion_queue.size() != size)
                            expansion_queue.pop_back();
                        break;
                    } else {
                        if (!state_info.is_value_initialized()) {
                            lookup_initialize(expansion_data.state, state_info);
                            continue;
                        }
                        // std::cout << ">" << state_id << " -> I"
                        //           << state_info.is_value_initialized() <<
                        //           "|G"
                        //           << state_info.is_goal_state() << "|D"
                        //           << state_info.is_dead_end() << "|R"
                        //           << state_info.is_recognized_dead_end() <<
                        //           "|T"
                        //           << state_info.is_terminal() << std::endl;
                        assert(state_info.is_value_initialized());
                        assert(!state_info.is_terminal());
                        PersistentStateData& pd = indices[state_id];
                        assert(pd.index == -1);
                        pd.index = expansion_data.lowlink = index++;
                        pd.onstack = true;
                        this->generate_applicable_ops(
                            expansion_data.state, expansion_data.aops);
                        if (expansion_data.aops.empty()) {
                            state_info.set_dead_end();
                            result.second = this->update(state_info, dead_end_value_);
                            continue;
                        }
                        stack.push_front(expansion_data.state);
                    }
                } else {
                    expansion_data.aops.pop_back();
                    if (expansion_data.aops.empty()) {
                        ExpansionData copy(std::move(expansion_data));
                        expansion_queue.pop_back();
                        if (copy.root) {
                            auto end = stack.begin();
                            do {
                                const StateID stack_state_id = *end;
                                auto& info = state_infos_[stack_state_id];
                                info.set_recognized_dead_end();
                                result.second = this->update(info, dead_end_value_)
                                    || result.second;
                                indices[stack_state_id].onstack = false;
                                statistics_.dead_end_safe_updates_dead_ends++;
                                // std::cout << " ---> dead: " << *end <<
                                // std::endl;
                                ++end;
                                if (stack_state_id == state_id) {
                                    break;
                                }
                            } while (true);
                            listener(stack.begin(), end);
                            stack.erase(stack.begin(), end);
                        } else {
                            assert(!expansion_queue.empty());
                            expansion_queue.back().update_lowlink(copy.lowlink);
                        }
                        continue;
                    }
                }
                // assert(is_equal_(state_info.get_value(), dead_end_value_));
                assert(!expansion_data.aops.empty());
                Distribution<StateID> transition_;
                this->generate_successors(
                    expansion_data.state,
                    expansion_data.aops.back(),
                    transition_);
                const int pidx = expansion_queue.size();
                // std::cout << " next successors = [";
                for (auto it = transition_.begin(); it != transition_.end();
                     ++it) {
                    StateID succ_id = it->first;
                    const auto& d = indices[succ_id];
                    // std::cout << " " << it->first << "(" << d.index << ")";
                    if (d.onstack) {
                        expansion_data.update_lowlink(d.index);
                    } else if (d.index == -1) {
                        expansion_queue.emplace_back(it->first, pidx);
                    }
                }
                // std::cout << " ]" << std::endl;
                transition_.clear();
            }
        }
        if constexpr (StorePolicy::value) {
            if (result.first) {
                while (!expansion_queue.empty()) {
                    auto& expansion_data = expansion_queue.back();
                    const StateID state_id = expansion_data.state;
                    StateInfo& state_info = state_infos_[state_id];
                    const ActionID pid =
                        this->get_action_id(
                            state_id, expansion_data.aops.back());
                    result.second =
                        result.second || (state_info.get_policy() != pid);
                    state_info.set_policy(pid);
                    const unsigned size = expansion_data.pidx;
                    while (expansion_queue.size() != size)
                        expansion_queue.pop_back();
                }
            }
        }
        if (result.first) {
            ++statistics_.wrongly_classified_dead_ends;
        }

        // std::cout << "==> result: " << result.first << ", " << result.second
        //           << std::endl;

        return result;
    }

protected:
    bool update(StateInfo& state_info, const IncumbentSolution& other)
    {
        if constexpr (DualBounds::value) {
            return value_utils::update_check(
                state_info.value, other, interval_comparison_);
        } else {
            return value_utils::update_check(state_info.value, other);
        }
    }


#if 0
    value_type::value_t create_result(const StateID& id)
    {
        const StateInfo& info = state_infos_[id];
        if constexpr (DualBounds::value) {
            statistics_.print_error = true;
            statistics_.error = info.error_bound();
        }
        return info.get_value();
    }
#endif

    void initialize_report(const State& state)
    {
        initial_state_id_ = this->get_state_id(state);

        static bool initialized = false;
        if (initialized)
            return;
        initialized = true;

        const StateInfo& info = lookup_initialize(this->get_state_id(state));
        // this->state_infos_[this->state_id_map_->operator[](state)];
        this->add_values_to_report(&info);
        statistics_.value = value_utils::as_upper_bound(info.value);
        statistics_.before_last_update = statistics_;
        statistics_.initial_state_estimate = value_utils::as_upper_bound(info.value);
        statistics_.initial_state_found_terminal = info.is_terminal();

        setup_custom_reports(state);
    }

    void report(const StateID) { this->report_->operator()(); }

    virtual void setup_custom_reports(const State&) { }

    storage::PerStateStorage<StateInfo>& get_state_info_store()
    {
        return state_infos_;
    }

    template<typename Info>
    bool do_bounds_disagree(const StateID& state_id, const Info& info)
    {
        if constexpr (DualBounds::value) {
            if constexpr (std::is_same_v<Info, StateInfo>) {
                return interval_comparison_ && !info.value.bounds_equal();
            } else {
                return interval_comparison_ && !state_infos_[state_id].value.bounds_equal();
            }
        } else {
            return false;
        }
    }

    template<typename StateToString>
    void dump_search_space(const std::string& file_name, StateToString* sstr)
    {
        struct ExpansionCondition {
            explicit ExpansionCondition(
                const MDPEngine<State, Action>* hs,
                storage::PerStateStorage<StateInfo>* infos)
                : hs_(hs)
                , infos_(infos)
            {
            }

            EvaluationResult operator()(const State& state) const
            {
                const StateID sid = hs_->get_state_id(state);
                const StateInfo& info = infos_->operator[](sid);
                return EvaluationResult(info.is_on_fringe(), 0);
            }

            const MDPEngine<State, Action>* hs_;
            storage::PerStateStorage<StateInfo>* infos_;
        };

        struct ActionToString {
            std::string operator()(const Action&) const { return ""; }
        };

        ExpansionCondition prune(this, &state_infos_);

        std::ofstream out;
        out.open(file_name);

        graphviz::GraphVisualization<
            State,
            Action,
            StateToString,
            ActionToString,
            StateEvaluator<State>,
            ExpansionCondition,
            ApplicableActionsGenerator<Action>,
            TransitionGenerator<Action>>
            gv(out,
               this->get_state_id_map(),
               this->get_state_reward_function(),
               this->get_applicable_actions_generator(),
               this->get_transition_generator(),
               sstr,
               nullptr,
               false,
               &prune);
        gv(this->lookup_state(initial_state_id_));

        out.close();
    }

    void dump_search_space(const std::string& file_name)
    {
        struct StateToString {
            std::string operator()(const State&) const { return ""; }
        };

        this->dump_search_space<StateToString>(file_name, nullptr);
    }

private:
    void add_values_to_report(const StateInfo* info)
    {
        if constexpr (DualBounds::value) {
            report_->register_value("vl", [info]() {
                return value_utils::as_lower_bound(info->value);
            });
            report_->register_value("vu", [info]() {
                return value_utils::as_upper_bound(info->value);
            });
        } else {
            report_->register_value("v", [info]() {
                return value_utils::as_upper_bound(info->value);
            });
        }
    }

    StateInfo& lookup_initialize(const StateID& state_id)
    {
        StateInfo& info = state_infos_[state_id];
        return lookup_initialize(state_id, info);
    }

    StateInfo& lookup_initialize(const StateID& state_id, StateInfo& state_info)
    {
        if (!state_info.is_value_initialized()) {
            statistics_.evaluated_states++;
            State state = this->lookup_state(state_id);
            auto estimate = this->get_state_reward(state);
            if (estimate) {
                state_info.set_goal();
                state_info.value = IncumbentSolution((value_type::value_t)estimate);
                statistics_.goal_states++;
                if (on_new_state_)
                    on_new_state_->touch_goal(state);
            } else {
                estimate = value_initializer_->operator()(state);
                if (estimate) {
                    statistics_.pruned_states++;
                    state_info.set_recognized_dead_end();
                    state_info.value = dead_end_value_;
                    if (on_new_state_)
                        on_new_state_->touch_dead_end(state);
                } else {
                    state_info.set_on_fringe();

                    if constexpr (DualBounds::value) {
                        state_info.value.upper =
                            static_cast<value_type::value_t>(estimate);
                    } else {
                        state_info.value =
                            static_cast<value_type::value_t>(estimate);
                    }

                    if (on_new_state_)
                        on_new_state_->touch(state);
                }
            }
        }
        return state_info;
    }

    IncumbentSolution dead_end_value() const
    {
        return dead_end_value_;
    }

    template<typename T>
    bool async_update(
        const bool stable_policy,
        const StateID& s,
        T* policy_tiebreaker,
        ActionID* greedy_action,
        Distribution<StateID>* greedy_transition,
        bool* action_changed)
    {
        if (policy_tiebreaker == nullptr) {
            return compute_value_update<false, false>(
                s,
                policy_tiebreaker,
                greedy_action,
                greedy_transition,
                action_changed);
        } else {
            if (stable_policy) {
                return compute_value_update<true, true>(
                    s,
                    policy_tiebreaker,
                    greedy_action,
                    greedy_transition,
                    action_changed);
            } else {
                return compute_value_update<true, false>(
                    s,
                    policy_tiebreaker,
                    greedy_action,
                    greedy_transition,
                    action_changed);
            }
        }
    }

    template<bool Policy, bool StablePolicy, typename T>
    bool compute_value_update(
        const StateID& state_id,
        [[maybe_unused]] T* choice,
        [[maybe_unused]] ActionID* greedy_action,
        [[maybe_unused]] Distribution<StateID>* greedy_transition,
        [[maybe_unused]] bool* action_changed)
    {
#if defined(EXPENSIVE_STATISTICS)
        statistics_.update_time.resume();
#endif
        statistics_.backups++;

        assert(!Policy || choice != nullptr);
        assert(greedy_transition == nullptr || greedy_transition->empty());

        StateInfo& state_info =
            lookup_initialize(state_id, state_infos_[state_id]);

        if (state_info.is_terminal()) {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.update_time.stop();
#endif
            return false;
        }

        if (state_info.is_on_fringe()) {
            ++statistics_.backed_up_states;
            state_info.removed_from_fringe();
        }

        std::vector<Action> aops_;
        std::vector<Distribution<StateID>> optimal_transitions_;
        this->generate_all_successors(state_id, aops_, optimal_transitions_);
        assert(aops_.size() == optimal_transitions_.size());

        if (aops_.empty()) {
            statistics_.terminal_states++;
            bool result = this->update(state_info, dead_end_value_);
            state_info.set_dead_end();
            state_info.set_policy(ActionID::undefined);
#if defined(EXPENSIVE_STATISTICS)
            statistics_.update_time.stop();
#endif
            if (result) {
                ++statistics_.value_changes;
                if (state_id == initial_state_id_) {
                    statistics_.jump();
                }
            }
            return result;
        }

        bool first = true;
        IncumbentSolution new_value = dead_end_value();
        std::vector<IncumbentSolution> values;
        values.reserve(aops_.size());

        // for (int i = aops_.size() - 1; i >= 0; i--) {
        unsigned non_loop_transitions = 0;
        for (unsigned i = 0; i < aops_.size(); ++i) {
            IncumbentSolution t_value(
                state_info.get_state_reward()
                + this->get_action_reward(state_id, aops_[i]));
            value_type::value_t self_loop = value_type::zero;
            bool non_loop = false;
            bool has_self_loop = false;
            const Distribution<StateID>& transition = optimal_transitions_[i];
            for (auto it = transition.begin(); it != transition.end(); ++it) {
                const StateID succ_id = it->first;
                if (succ_id == state_id) {
                    has_self_loop = true;
                    self_loop += it->second;
                } else {
                    const StateInfo& succ_info =
                        lookup_initialize(succ_id, state_infos_[succ_id]);
                    t_value += it->second * succ_info.value;
                    non_loop = true;
                }
            }
            if (non_loop) {
                if (has_self_loop) {
                    t_value *= value_type::one / (value_type::one - self_loop);
                }

                values.push_back(t_value);
                if (first) {
                    first = false;
                    new_value = t_value;
                } else {
                    value_utils::update(new_value, t_value);
                }
                if (non_loop_transitions != i) {
                    optimal_transitions_[i].swap(
                        optimal_transitions_[non_loop_transitions]);
                    std::swap(aops_[i], aops_[non_loop_transitions]);
                }
                ++non_loop_transitions;
            }
        }

#if defined(EXPENSIVE_STATISTICS)
        statistics_.update_time.stop();
#endif
        // if (value_type::approx_equal(value_type::eps)(state_info.value, this->get_minimal_reward())) {
        //     non_loop_transitions = 0;
        // }

        if (non_loop_transitions == 0) {
            statistics_.self_loop_states++;
            const bool result = this->update(state_info, dead_end_value_);
            state_info.set_policy(ActionID::undefined);
            state_info.set_dead_end();
            return result;
            // assert(is_equal_(new_value, dead_end_value_));
        } 
        
        if constexpr (Policy) {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.policy_selection_time.resume();
#endif

            int index = -1;

            unsigned j = 0;
            for (unsigned i = 0; i < non_loop_transitions; ++i) {
                if (value_utils::compare(values[i], new_value) >= 0) {
                    if (i != j) {
                        optimal_transitions_[j].swap(optimal_transitions_[i]);
                        std::swap(aops_[j], aops_[i]);
                    }
                    if (StablePolicy
                        && this->get_action_id(state_id, aops_[j])
                            == state_info.get_policy()) {
                        index = j++;
                        break;
                    }
                    ++j;
                }
            }

            if (j < optimal_transitions_.size()) {
                aops_.erase(aops_.begin() + j, aops_.end());
                optimal_transitions_.erase(
                    optimal_transitions_.begin() + j,
                    optimal_transitions_.end());
            }

            if (index == -1) {
                ++statistics_.policy_updates;
                assert(!optimal_transitions_.empty());
                index = choice->operator()(
                    state_id,
                    state_info.get_policy(),
                    aops_,
                    optimal_transitions_);
                assert(index < 0 || index < static_cast<int>(aops_.size()));
            }

            if (index >= 0) {
                const ActionID action_id =
                    this->get_action_id(state_id, aops_[index]);

                if (action_changed != nullptr) {
                    *action_changed = (action_id != state_info.get_policy());
                }

                if (greedy_action != nullptr) {
                    (*greedy_action) = action_id;
                }

                if (greedy_transition != nullptr) {
                    greedy_transition->swap(optimal_transitions_[index]);
                }

                state_info.set_policy(action_id);
            }

#if defined(EXPENSIVE_STATISTICS)
            statistics_.policy_selection_time.stop();
#endif
        }

        if (this->update(state_info, new_value)) {
            ++statistics_.value_changes;
            if (state_id == initial_state_id_) {
                statistics_.jump();
            }
            return true;
        }
        return false;
    }

protected:
    ProgressReport* report_;
    const bool interval_comparison_;
    const bool stable_policy_;

private:
    StateEvaluator<State>* value_initializer_;
    PolicyPicker<Action>* policy_chooser_;
    NewStateHandler<State>* on_new_state_;
    DeadEndListener<State, Action>* dead_end_listener_;
    StateEvaluator<State>* dead_end_eval_;

    const IncumbentSolution dead_end_value_;

    storage::PerStateStorage<StateInfo> state_infos_;

    Statistics statistics_;

    StateID initial_state_id_;
};

} // namespace internal

template<typename T>
struct NoAdditionalStateData : public T {
};

template<
    typename State,
    typename Action,
    typename DualValueBounds = std::false_type,
    typename StorePolicy = std::false_type,
    template<typename> class StateInfo = NoAdditionalStateData>
using HeuristicSearchBase = internal::HeuristicSearchBase<
    State,
    Action,
    StateInfo<PerStateBaseInformation<StorePolicy, DualValueBounds>>>;

} // namespace heuristic_search
} // namespace engines
} // namespace probabilistic
