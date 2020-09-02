#pragma once

#include "../analysis_result.h"
#include "../distribution.h"
#include "../evaluation_result.h"
#include "../interfaces/i_engine.h"
#include "../interfaces/i_printable.h"
#include "../progress_report.h"
#include "../value_type.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"
#include "value_utils.h"

#if defined(EXPENSIVE_STATISTICS)
#include "../../utils/timer.h"
#endif

#include <cassert>
#include <deque>
#include <iostream>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace algorithms {
namespace heuristic_search_base {

struct HeuristicSearchStateBaseInformation {
    static constexpr const uint8_t INITIALIZED = 1;
    static constexpr const uint8_t DEAD = 2;
    static constexpr const uint8_t RECOGNIZED_DEAD = 3;
    static constexpr const uint8_t GOAL = 4;
    static constexpr const uint8_t MASK = 7;
    static constexpr const uint8_t BITS = 3;

    bool is_value_initialized() const { return (MASK & info) != 0; }
    bool is_dead_end() const { return info & DEAD; }
    bool is_recognized_dead_end() const
    {
        return (info & MASK) == RECOGNIZED_DEAD;
    }
    bool is_goal_state() const { return (MASK & info) == GOAL; }
    bool is_terminal() const { return info & (GOAL | DEAD); }

    void set_value_initialized()
    {
        assert(is_dead_end());
        info = (info & ~MASK) | INITIALIZED;
    }

    void set_dead_end()
    {
        assert(
            is_value_initialized() && !is_goal_state()
            && !is_recognized_dead_end());
        info = (info & ~MASK) | DEAD;
    }

    void set_recognized_dead_end()
    {
        assert(is_value_initialized() && !is_goal_state());
        info = (info & ~MASK) | RECOGNIZED_DEAD;
    }

    const value_type::value_t& get_value() const { return value; }

    const value_type::value_t& get_state_reward() const
    {
        assert(is_value_initialized());
        return reward;
    }

    HeuristicSearchStateBaseInformation()
        : info(0)
        , reward(value_type::zero)
        , value(value_type::zero)
    {
    }

    uint8_t info;
    value_type::value_t reward;
    value_type::value_t value;
};

template<typename Action>
struct StatePolicyField {
    StatePolicyField()
        : policy(NullAction<Action>()())
    {
    }
    Action policy;
    using StoresPolicy = std::true_type;
};

template<>
struct StatePolicyField<void> {
    using StoresPolicy = std::false_type;
};

template<typename T = std::false_type>
class HeuristicSearchStateInformation
    : public HeuristicSearchStateBaseInformation {
public:
    using DualBounds = std::false_type;

    void initialize_value(const EvaluationResult& reward)
    {
        assert(!is_value_initialized());
        this->reward = (value_type::value_t)reward;
        if (reward) {
            this->info = (this->info & ~MASK) | GOAL;
            this->value = (value_type::value_t)reward;
        } else {
            this->info = (this->info & ~MASK) | INITIALIZED;
        }
    }

    void
    set_value(const value_utils::IncumbentSolution<std::false_type>& new_value)
    {
        value = new_value.first;
    }

    bool update_value(
        const bool,
        const value_utils::IncumbentSolution<std::false_type>& new_value)
    {
        const bool result = !value_type::approx_equal()(value, new_value.first);
        value = new_value.first;
        return result;
    }

    bool bounds_equal() const { return true; }

    value_type::value_t error_bound() const { return value_type::zero; }
};

template<>
class HeuristicSearchStateInformation<std::true_type>
    : public HeuristicSearchStateBaseInformation {
public:
    using DualBounds = std::true_type;

    HeuristicSearchStateInformation()
        : HeuristicSearchStateBaseInformation()
        , value2(value_type::zero)
    {
    }

    void initialize_value(const EvaluationResult& reward)
    {
        assert(!is_value_initialized());
        this->reward = (value_type::value_t)reward;
        if (reward) {
            this->info = (this->info & ~MASK) | GOAL;
            this->value = (value_type::value_t)reward;
            this->value2 = (value_type::value_t)reward;
        } else {
            this->info = (this->info & ~MASK) | INITIALIZED;
        }
    }

    void
    set_value(const value_utils::IncumbentSolution<std::false_type>& new_value)
    {
        value = new_value.first;
        value2 = new_value.first;
    }

    void
    set_value(const value_utils::IncumbentSolution<std::true_type>& new_value)
    {
        value = new_value.first;
        value2 = new_value.first;
    }

    bool update_value(
        const bool x,
        const value_utils::IncumbentSolution<std::true_type>& new_value)
    {
        const bool result = !value_type::approx_equal()(value, new_value.first)
            || (x && !value_type::approx_equal()(value2, new_value.second));
        value = new_value.first;
        value2 = new_value.second;
        return result;
    }

    bool bounds_equal() const
    {
        return value_type::approx_equal()(value, value2);
    }

    value_type::value_t error_bound() const { return value - value2; }

    value_type::value_t value2;
};

namespace internal {

struct CoreStatistics {
    unsigned long long backups = 0;
    unsigned long long transition_generations = 0;
    unsigned long long state_generations = 0;

    unsigned long long evaluated_states = 0;
    unsigned long long pruned_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long self_loop_states = 0;
};

struct Statistics : public CoreStatistics, public IPrintable {

    unsigned state_info_bytes = 0;
    value_type::value_t initial_state_estimate = 0;
    bool initial_state_found_terminal = 0;

    unsigned long long externally_set_dead_ends = 0;
    unsigned long long dead_end_safe_updates = 0;
    unsigned long long dead_end_safe_updates_states = 0;
    unsigned long long dead_end_safe_updates_dead_ends = 0;

    unsigned long long wrongly_classified_dead_ends = 0;
    unsigned long long safe_updates_non_dead_end_value = 0;

    bool print_error = false;
    value_type::value_t error = value_type::zero;

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

    void notify_value(const value_type::value_t& new_value)
    {
        if (!value_type::approx_equal()(new_value, value)) {
            before_last_update = *this;
        }
        value = new_value;
    }

    virtual void print(std::ostream& out) const override
    {
        out << "Stored " << state_info_bytes << " bytes per state" << std::endl;

        out << "Initial state value estimation: " << initial_state_estimate
            << std::endl;
        out << "Initial state value found terminal: "
            << initial_state_found_terminal << std::endl;

        out << "Evaluated state(s): " << evaluated_states << std::endl;
        out << "Pruned state(s): " << pruned_states << std::endl;
        out << "Goal state(s): " << goal_states << std::endl;
        out << "Terminal state(s): " << terminal_states << std::endl;
        out << "Self-loop state(s): " << self_loop_states << std::endl;
        out << "Generated transition(s): " << transition_generations
            << std::endl;
        out << "Generated state(s): " << state_generations << std::endl;
        out << "Number of backups: " << backups << std::endl;

        out << "Evaluated state(s) until last value change: "
            << before_last_update.evaluated_states << std::endl;
        out << "Pruned state(s) until last value change: "
            << before_last_update.pruned_states << std::endl;
        out << "Goal state(s) until last value change: "
            << before_last_update.goal_states << std::endl;
        out << "Terminal state(s) until last value change: "
            << before_last_update.terminal_states << std::endl;
        out << "Self-loop state(s) until last value change: "
            << before_last_update.self_loop_states << std::endl;
        out << "Generated transition(s) until last value change: "
            << before_last_update.transition_generations << std::endl;
        out << "Generated state(s) until last value change: "
            << before_last_update.state_generations << std::endl;
        out << "Number of backups until last value change: "
            << before_last_update.backups << std::endl;

#if defined(EXPENSIVE_STATISTICS)
        out << "Updating time: " << update_time << std::endl;
        out << "Policy selection time: " << policy_selection_time << std::endl;
#endif

        if (print_error) {
            out << "Final error: " << error << std::endl;
        }

        out << "Number of dead-end identification runs: "
            << dead_end_safe_updates << std::endl;
        out << "Dead-end identification runs on states with non dead-end "
               "value: "
            << safe_updates_non_dead_end_value << std::endl;
        out << "States considered while checking dead-end property: "
            << dead_end_safe_updates_states << std::endl;
        out << "Wrongly classified dead-ends: " << wrongly_classified_dead_ends
            << std::endl;
        out << "Dead-ends identified: " << dead_end_safe_updates_dead_ends
            << std::endl;
        out << "Externally set dead-ends: " << externally_set_dead_ends
            << std::endl
            << std::flush;
    }
};

template<typename Action, typename DualBounds>
struct PerStateInformation : public HeuristicSearchStateInformation<DualBounds>,
                             public StatePolicyField<Action> {
};

template<typename StateInfo, typename Action, typename C = std::false_type>
struct set_policy_ {
    void operator()(StateInfo&, const Action&) const { }
};

template<typename StateInfo, typename Action>
struct set_policy_<StateInfo, Action, std::true_type> {
    void operator()(StateInfo& info, const Action& a) const { info.policy = a; }
};

template<typename StateInfo, typename Action>
using set_policy =
    set_policy_<StateInfo, Action, typename StateInfo::StoresPolicy>;

template<typename StateInfo, typename Action, typename C = std::false_type>
struct get_policy_ptr_ {
    const Action* operator()(const StateInfo&) const { return nullptr; }
};

template<typename StateInfo, typename Action>
struct get_policy_ptr_<StateInfo, Action, std::true_type> {
    const Action* operator()(const StateInfo& info) const
    {
        return &info.policy;
    }
};

template<typename StateInfo, typename Action>
using get_policy_ptr =
    get_policy_ptr_<StateInfo, Action, typename StateInfo::StoresPolicy>;

} // namespace internal

namespace internal {

template<typename RealStateInfo, typename State, typename Action>
class HeuristicSearchStatusInterface
    : public algorithms::HeuristicSearchStatusInterface<State, Action> {
public:
    HeuristicSearchStatusInterface(
        StateIDMap<State>* state_id_map,
        storage::PerStateStorage<RealStateInfo>* infos)
        : state_id_map_(state_id_map)
        , infos_(infos)
    {
    }

    virtual const HeuristicSearchStateBaseInformation*
    lookup_state_info(const State& state) override
    {
        return &infos_->operator[](state_id_map_->operator[](state));
    }

private:
    StateIDMap<State>* state_id_map_;
    storage::PerStateStorage<RealStateInfo>* infos_;
};

template<typename StateT, typename ActionT, typename StateInfoT>
class HeuristicSearchBase : public IMDPEngine<StateT> {
public:
    using State = StateT;
    using Action = ActionT;
    using StateInfo = StateInfoT;
    using StorePolicy = typename StateInfo::StoresPolicy;
    using DualBounds = typename StateInfo::DualBounds;

    template<typename T>
    class StateStatusAccessor {
    public:
        StateInfo& operator()(const StateID& state_id)
        {
            return infos_->lookup(state_id);
        }

        StateInfo& operator()(const State& state)
        {
            return infos_->lookup(state);
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
            return infos_->lookup(id);
        }

        HeuristicSearchBase* infos_;
    };

    HeuristicSearchBase(
        ProgressReport* report,
        StateIDMap<State>* state_id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        StateEvaluationFunction<State>* value_init,
        PolicyChooser<State, Action>* policy_chooser,
        value_type::value_t dead_end_value,
        NewStateHandler<State, Action>* new_state_handler,
        bool interval_comparison = false)
        : report_(report)
        , state_id_map_(state_id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
        , interval_comparison_(interval_comparison)
        , state_reward_(state_reward)
        , transition_reward_(transition_reward)
        , value_initializer_(value_init)
        , policy_chooser_(policy_chooser)
        , dead_end_value_(dead_end_value)
        , on_new_state_(new_state_handler)
        , interface_(state_id_map, &state_infos_)
    {
        statistics_.state_info_bytes = sizeof(StateInfo);
        if (policy_chooser_) {
            policy_chooser_->connect(&interface_);
        }
    }

    virtual ~HeuristicSearchBase() = default;

    algorithms::HeuristicSearchStatusInterface<State, Action>* interface()
    {
        return &interface_;
    }

    bool apply_policy(const State& state, Distribution<State>& result)
    {
        const StateID stateid = this->state_id_map_->operator[](state);
        StateInfo& info = state_infos_[stateid];
        return apply_policy(StorePolicy(), state, info, result);
    }

    inline bool async_update(const State& state)
    {
        return async_update(state, policy_chooser_, nullptr, nullptr, nullptr);
    }

    inline bool async_update(
        const State& s,
        Action* greedy_action,
        Distribution<State>* transition)
    {
        return async_update(
            s, policy_chooser_, greedy_action, transition, nullptr);
    }

    inline bool async_update(
        const State& s,
        Action* greedy_action,
        Distribution<State>* transition,
        bool* action_changed)
    {
        return async_update(
            s, policy_chooser_, greedy_action, transition, action_changed);
    }

    template<typename T>
    inline bool async_update(
        const State& s,
        T* policy_tiebreaker,
        Action* greedy_action,
        Distribution<State>* greedy_transition,
        bool* action_changed)
    {
        if (policy_tiebreaker == nullptr) {
            return compute_value_update<DualBounds>(
                std::false_type(),
                s,
                policy_tiebreaker,
                greedy_action,
                greedy_transition,
                action_changed);
        } else {
            return compute_value_update<DualBounds>(
                std::true_type(),
                s,
                policy_tiebreaker,
                greedy_action,
                greedy_transition,
                action_changed);
        }
    }

    template<typename DeadEndListener, typename AlternativeCondition>
    bool safe_async_update(
        const State& s,
        DeadEndListener& listener,
        AlternativeCondition& alt_cond,
        const bool update_policy = StorePolicy::value)
    {
        std::pair<bool, bool> result;
        this->safe_async_update(s, listener, alt_cond, result, update_policy);
        return result.first || result.second;
    }

    template<typename DeadEndListener, typename AlternativeCondition>
    void safe_async_update(
        const State& s,
        DeadEndListener& listener,
        AlternativeCondition& alt_cond,
        std::pair<bool, bool>& result,
        const bool update_policy = StorePolicy::value)
    {
        // std::cout << "safe_async_update(" << state_id_map_->operator[](s) <<
        // ")"
        //           << std::endl;

        struct ExpansionData {
            ExpansionData(const State& s, int pidx)
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
            State state;
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
        result = std::pair<bool, bool>(false, false);

        int index = 0;
        storage::StateHashMap<PersistentStateData> indices;
        std::deque<ExpansionData> expansion_queue;
        std::deque<State> stack;
        expansion_queue.emplace_back(s, 0);
        while (!expansion_queue.empty()) {
            assert(!result.first);
            auto& expansion_data = expansion_queue.back();
            const StateID state_id =
                this->state_id_map_->operator[](expansion_data.state);
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
                state_info.set_value(dead_end_value_);
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
                        aops_gen_->operator()(
                            expansion_data.state, expansion_data.aops);
                        if (expansion_data.aops.empty()) {
                            state_info.set_dead_end();
                            result.second = state_info.update_value(
                                interval_comparison_, dead_end_value_);
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
                                const StateID stack_state_id =
                                    this->state_id_map_->operator[](*end);
                                auto& info = state_infos_[stack_state_id];
                                info.set_recognized_dead_end();
                                result.second =
                                    info.update_value(
                                        interval_comparison_, dead_end_value_)
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
                transition_ = transition_gen_->operator()(
                    expansion_data.state, expansion_data.aops.back());
                const int pidx = expansion_queue.size();
                // std::cout << " next successors = [";
                for (auto it = transition_.begin(); it != transition_.end();
                     ++it) {
                    StateID succ_id =
                        this->state_id_map_->operator[](it->first);
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
        if (update_policy && result.first) {
            while (!expansion_queue.empty()) {
                auto& expansion_data = expansion_queue.back();
                const StateID state_id =
                    state_id_map_->operator[](expansion_data.state);
                StateInfo& state_info = state_infos_[state_id];
                if (!result.second) {
                    const Action* pol =
                        get_policy_ptr<StateInfo, Action>()(state_info);
                    result.second = !(*pol == expansion_data.aops.back());
                }
                set_policy<StateInfo, Action>()(
                    state_info, expansion_data.aops.back());
                const unsigned size = expansion_data.pidx;
                while (expansion_queue.size() != size)
                    expansion_queue.pop_back();
            }
        }
        if (result.first) {
            ++statistics_.wrongly_classified_dead_ends;
        }

        // std::cout << "==> result: " << result.first << ", " << result.second
        //           << std::endl;
    }

    bool is_terminal(const StateID& state_id)
    {
        return state_infos_[state_id].is_terminal();
    }

    void set_dead_end(StateInfo& info)
    {
        info.set_recognized_dead_end();
        info.set_value(dead_end_value_);
    }

    void set_dead_end(const StateID& state_id)
    {
        StateInfo& info = state_infos_[state_id];
        info.set_recognized_dead_end();
        info.set_value(dead_end_value_);
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

    IPrintable* get_base_statistics() { return new Statistics(statistics_); }

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

protected:
    AnalysisResult
    create_result(const State& s, IPrintable* statistics = nullptr)
    {
        StateID id = this->state_id_map_->operator[](s);
        const StateInfo& info = state_infos_[id];
        if (DualBounds::value) {
            statistics_.print_error = true;
            statistics_.error = info.error_bound();
        }
        if (statistics == nullptr) {
            return AnalysisResult(
                info.get_value(), new Statistics(statistics_));
        } else {
            return AnalysisResult(
                info.get_value(),
                new IPrintables({ statistics, new Statistics(statistics_) }));
        }
    }

    void add_values_to_report(const std::true_type&, const StateInfo* info)
    {
        report_->register_value("vl", [info]() { return info->value2; });
        report_->register_value("vu", [info]() { return info->value; });
    }

    void add_values_to_report(const std::false_type&, const StateInfo* info)
    {
        report_->register_value("v", [info]() { return info->value; });
    }

    void initialize_report(const State& state)
    {
        static bool initialized = false;
        if (initialized)
            return;
        initialized = true;

        const StateInfo& info = lookup_initialize(state);
        // this->state_infos_[this->state_id_map_->operator[](state)];
        this->add_values_to_report(DualBounds(), &info);
        statistics_.value = info.get_value();
        statistics_.before_last_update = statistics_;
        statistics_.initial_state_estimate = info.get_value();
        statistics_.initial_state_found_terminal = info.is_terminal();

        setup_custom_reports(state);
    }

    void report(const StateID state_id)
    {
        const StateInfo& info = state_infos_[state_id];
        statistics_.notify_value(info.get_value());
        this->report_->operator()();
    }

    virtual void setup_custom_reports(const State&) { }

    storage::PerStateStorage<StateInfo>& get_state_info_store()
    {
        return state_infos_;
    }

    bool apply_policy(
        const State& state,
        StateInfo& info,
        Distribution<State>& result)
    {
        return this->apply_policy(StorePolicy(), state, info, result);
    }

    bool apply_policy(
        const std::true_type&,
        const State& state,
        StateInfo& info,
        Distribution<State>& result)
    {
        if (!info.is_value_initialized()) {
            async_update(state, nullptr, &result);
        } else {
            result = transition_gen_->operator()(state, info.policy);
        }
        return true;
    }

    bool apply_policy(
        const std::false_type&,
        const State&,
        const StateInfo&,
        const Distribution<State>&);

    bool has_dead_end_value(const State& state)
    {
        const StateInfo& info = lookup(state);
        return info.value == dead_end_value_.first;
    }

private:
    template<typename T>
    friend class StateStatusAccessor;

    StateInfo& lookup(const State& state)
    {
        StateID state_id = state_id_map_->operator[](state);
        return state_infos_[state_id];
    }

    StateInfo& lookup(const StateID& state_id)
    {
        return state_infos_[state_id];
    }

    StateInfo& lookup_initialize(const State& state)
    {
        StateID state_id = state_id_map_->operator[](state);
        StateInfo& info = state_infos_[state_id];
        return lookup_initialize(state, info);
    }

    StateInfo& lookup_initialize(const StateID& state_id)
    {
        State state = state_id_map_->operator[](state_id);
        StateInfo& info = state_infos_[state_id];
        return lookup_initialize(state, info);
    }

    StateInfo& lookup_initialize(const State& state, StateInfo& state_info)
    {
        if (!state_info.is_value_initialized()) {
            statistics_.evaluated_states++;
            auto estimate = state_reward_->operator()(state);
            state_info.initialize_value(estimate);
            if (estimate) {
                statistics_.goal_states++;
                if (on_new_state_)
                    on_new_state_->touch_goal(state);
            } else {
                estimate = value_initializer_->operator()(state);
                if (estimate) {
                    statistics_.pruned_states++;
                    state_info.set_recognized_dead_end();
                    state_info.set_value(dead_end_value_);
                    if (on_new_state_)
                        on_new_state_->touch_dead_end(state);
                } else {
                    state_info.value = (value_type::value_t)estimate;
                    if (on_new_state_)
                        on_new_state_->touch(state);
                }
            }
        }
        return state_info;
    }

    inline value_utils::IncumbentSolution<std::true_type>
    dead_end_value(const std::true_type&) const
    {
        return value_utils::IncumbentSolution<std::true_type>(
            dead_end_value_.first, dead_end_value_.first);
    }

    inline value_utils::IncumbentSolution<std::false_type>
    dead_end_value(const std::false_type&) const
    {
        return value_utils::IncumbentSolution<std::false_type>(
            dead_end_value_.first);
    }

    template<typename Update2, typename T, bool Policy>
    bool compute_value_update(
        const std::integral_constant<bool, Policy>&,
        const State& s,
        T* choice,
        Action* greedy_action,
        Distribution<State>* greedy_transition,
        bool* action_changed)
    {
#if defined(EXPENSIVE_STATISTICS)
        statistics_.update_time.resume();
#endif
        statistics_.backups++;
        assert(
            aops_.empty() && transition_.empty()
            && optimal_transitions_.empty());
        assert(!Policy || choice != nullptr);
        assert(greedy_transition == nullptr || greedy_transition->empty());
        StateID state_id = this->state_id_map_->operator[](s);
        auto& state_info = state_infos_[state_id];
        const bool unseen_state = !state_info.is_value_initialized();
        if (unseen_state) {
            lookup_initialize(s, state_info);
            assert(
                state_info.is_goal_state()
                == (bool)state_reward_->operator()(s));
        }
        if (state_info.is_terminal()) {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.update_time.stop();
#endif
            return false;
        }
        bool result = false;
        aops_gen_->operator()(s, aops_);
        if (aops_.empty()) {
            statistics_.terminal_states++;
            state_info.set_dead_end();
            result =
                state_info.update_value(interval_comparison_, dead_end_value_);
#if defined(EXPENSIVE_STATISTICS)
            statistics_.update_time.stop();
#endif
            return result;
        }
        statistics_.transition_generations += aops_.size();
        value_utils::IncumbentSolution<Update2> new_value =
            dead_end_value(Update2());
        bool all_self_loops = true;
        bool first = true;
        for (int i = aops_.size() - 1; i >= 0; i--) {
            transition_ = transition_gen_->operator()(s, aops_[i]);
            statistics_.state_generations += transition_.size();
            value_utils::IncumbentSolution<Update2> t_value =
                value_utils::IncumbentSolution<Update2>(
                    state_info.get_state_reward()
                    + transition_reward_->operator()(s, aops_[i]));
            value_type::value_t self_loop = value_type::zero;
            bool non_loop = false;
            bool has_self_loop = false;
            for (auto it = transition_.begin(); it != transition_.end(); ++it) {
                StateID succ_id = this->state_id_map_->operator[](it->first);
                if (succ_id == state_id) {
                    has_self_loop = true;
                    self_loop += it->second;
                } else {
                    const StateInfo& succ_info =
                        lookup_initialize(it->first, state_infos_[succ_id]);
                    value_utils::add(t_value, it->second, succ_info);
                    non_loop = true;
                }
            }
            if (non_loop) {
                all_self_loops = false;
                if (has_self_loop) {
                    value_utils::mult(
                        t_value,
                        value_type::one / (value_type::one - self_loop));
                }
                if (first) {
                    new_value = t_value;
                    if (Policy) {
                        optimal_transitions_.emplace_back(
                            aops_[i], std::move(transition_));
                    }
                    first = false;
                } else {
                    const int cmpr =
                        value_utils::update_incumbent(new_value, t_value);
                    if (Policy && cmpr >= 0) {
                        if (cmpr == 1) {
                            optimal_transitions_.clear();
                        }
                        optimal_transitions_.emplace_back(
                            aops_[i], std::move(transition_));
                    }
                }
            }
            transition_.clear();
        }
#if defined(EXPENSIVE_STATISTICS)
        statistics_.update_time.stop();
#endif
        aops_.clear();
        if (all_self_loops) {
            statistics_.self_loop_states++;
            state_info.set_dead_end();
            // assert(is_equal_(new_value, dead_end_value_));
        } else if (Policy) {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.policy_selection_time.resume();
#endif
            assert(!optimal_transitions_.empty());
            int x = choice->operator()(
                unseen_state,
                get_policy_ptr<StateInfo, Action>()(state_info),
                s,
                optimal_transitions_);
            if (x >= 0) {
                if (action_changed != nullptr) {
                    *action_changed =
                        *get_policy_ptr<StateInfo, Action>()(state_info)
                        != optimal_transitions_[x].first;
                }
                if (StorePolicy::value) {
                    set_policy<StateInfo, Action>()(
                        state_info, optimal_transitions_[x].first);
                }
                if (greedy_action != nullptr) {
                    (*greedy_action) = optimal_transitions_[x].first;
                }
                if (greedy_transition != nullptr) {
                    greedy_transition->swap(optimal_transitions_[x].second);
                }
            }
            optimal_transitions_.clear();
#if defined(EXPENSIVE_STATISTICS)
            statistics_.policy_selection_time.stop();
#endif
        }
        result = state_info.update_value(interval_comparison_, new_value);
        return result;
    }

protected:
    ProgressReport* report_;
    StateIDMap<State>* state_id_map_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    TransitionGenerator<State, Action>* transition_gen_;

    const bool interval_comparison_;

private:
    StateEvaluationFunction<State>* state_reward_;
    TransitionRewardFunction<State, Action>* transition_reward_;
    StateEvaluationFunction<State>* value_initializer_;
    PolicyChooser<State, Action>* policy_chooser_;
    const value_utils::IncumbentSolution<DualBounds> dead_end_value_;

    Statistics statistics_;
    storage::PerStateStorage<StateInfo> state_infos_;
    std::vector<Action> aops_;
    Distribution<State> transition_;
    std::vector<std::pair<Action, Distribution<State>>> optimal_transitions_;

    NewStateHandler<State, Action>* on_new_state_;
    HeuristicSearchStatusInterface<StateInfo, State, Action> interface_;
};

template<typename B, typename Action>
struct policy_type {
};
template<typename Action>
struct policy_type<std::true_type, Action> {
    using type = Action;
};
template<typename Action>
struct policy_type<std::false_type, Action> {
    using type = void;
};

} // namespace internal

template<typename T>
struct NoAdditionalStateData : public T {
};

template<
    typename State,
    typename Action,
    typename Bounds2 = std::false_type,
    typename StorePolicy = std::false_type,
    template<typename> class StateInfo = NoAdditionalStateData>
using HeuristicSearchBase = internal::HeuristicSearchBase<
    State,
    Action,
    StateInfo<internal::PerStateInformation<
        typename internal::policy_type<StorePolicy, Action>::type,
        Bounds2>>>;

} // namespace heuristic_search_base
} // namespace algorithms
} // namespace probabilistic
