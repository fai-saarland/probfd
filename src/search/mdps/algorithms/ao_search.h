#pragma once

#include "../distribution.h"
#include "../value_type.h"
#include "heuristic_search_base.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"

#include <queue>
#include <string>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace algorithms {
namespace ao_search {

namespace internal {

struct Statistics : public IPrintable {
    const std::string name;
    unsigned long long iterations = 0;

    explicit Statistics(const std::string& name)
        : name(name)
    {
    }

    virtual void print(std::ostream& out) const override
    {
        out << "**** " << name << " ****" << std::endl;
        out << "Iterations: " << iterations << std::endl;
    }

    void register_report(ProgressReport& report)
    {
        report.register_print(
            [this](std::ostream& out) { out << "i=" << iterations; });
    }
};

template<typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr const uint8_t BITS = StateInfo::BITS + 2;
    static constexpr const uint8_t MASK = 3 << StateInfo::BITS;
    static constexpr const uint8_t MARK = 1 << StateInfo::BITS;
    static constexpr const uint8_t SOLVED = 2 << StateInfo::BITS;

    bool is_tip_state() const { return update_order == 0; }
    bool is_marked() const { return this->info & MARK; }
    bool is_solved() const { return this->info & SOLVED; }
    bool is_unflagged() const { return (this->info & MASK) == 0; }

    void mark()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARK;
    }
    void unmark()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK);
    }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }

    const std::vector<StateID>& get_parents() const { return parents; }
    std::vector<StateID>& get_parents() { return parents; }
    void add_parent(const StateID& s) { parents.push_back(s); }

    PerStateInformation()
        : StateInfo()
        , update_order(0)
    {
    }

    std::vector<StateID> parents;
    int update_order;
};
} // namespace internal

template<
    typename StateT,
    typename ActionT,
    typename DualBounds,
    typename StorePolicy,
    template<typename>
    class StateInfoExtension,
    bool Greedy>
class AOBase : public heuristic_search_base::HeuristicSearchBase<
                   StateT,
                   ActionT,
                   DualBounds,
                   StorePolicy,
                   StateInfoExtension> {
public:
    using HeuristicSearchBase = heuristic_search_base::HeuristicSearchBase<
        StateT,
        ActionT,
        DualBounds,
        StorePolicy,
        StateInfoExtension>;
    using State = typename HeuristicSearchBase::State;
    using Action = typename HeuristicSearchBase::Action;

    template<typename... Args>
    AOBase(
        const std::string& name,
        StateListener<State, Action>* listener,
        DeadEndIdentificationLevel level,
        Args... args)
        : HeuristicSearchBase(args...)
        , state_infos_(this->template get_state_status_access<StateInfo>())
        , listener_(listener)
        , dead_end_ident_level_(level)
        , stats_(name)
        , mark_dead_ends_(this)
        , expansion_condition_(state_infos_, level)
    {
    }

protected:
    using StateInfo = typename HeuristicSearchBase::StateInfo;
    using StateStatusAccessor =
        typename HeuristicSearchBase::template StateStatusAccessor<StateInfo>;

    virtual void setup_custom_reports(const State&) override
    {
        this->stats_.register_report(*this->report_);
    }

    AnalysisResult create_result(const State& state)
    {
        return HeuristicSearchBase::create_result(
            state, new internal::Statistics(stats_));
    }

    void backpropagate_tip_value()
    {
        while (!queue_.empty()) {
            auto elem = queue_.top();
            queue_.pop();

            auto& info = state_infos_(elem.second);
            assert(!info.is_goal_state());
            assert(!info.is_terminal() || info.is_solved());
            if (info.is_solved()) {
                // has been handled already
                continue;
            }
            assert(info.is_marked());
            info.unmark();

            State state = this->state_id_map_->operator[](elem.second);
            bool solved = false;
            bool dead = false;
            bool value_changed = update_value_check_solved(
                IsGreedy(), state, info, solved, dead);
            if (solved) {
                mark_solved_push_parents(IsGreedy(), state, info, dead);
            } else if (value_changed) {
                push_parents_to_queue(info);
            }
        }
    }

    void backpropagate_update_order(const StateID& tip)
    {
        queue_.emplace(state_infos_(tip).update_order, tip);
        while (!queue_.empty()) {
            auto elem = queue_.top();
            queue_.pop();
            auto& info = state_infos_(elem.second);
            if (info.update_order > elem.first) {
                continue;
            }
            auto& parents = info.get_parents();
            auto it = parents.begin();
            while (it != parents.end()) {
                auto& pinfo = state_infos_(*it);
                if (pinfo.is_solved()) {
                    it = parents.erase(it);
                    continue;
                } else if (pinfo.update_order <= elem.first) {
                    pinfo.update_order = elem.first + 1;
                    queue_.emplace(elem.first + 1, *it);
                }
                ++it;
            }
        }
    }

    void initialize_tip_state_value(
        const State& state,
        StateInfo& info,
        bool& terminal,
        bool& solved,
        bool& dead,
        bool& value_changed)
    {
        assert(!info.is_solved());
        assert(info.is_tip_state());
        assert(queue_.empty());

        terminal = false;
        solved = false;
        value_changed =
            update_value_check_solved(IsGreedy(), state, info, solved, dead);

        if (info.is_terminal()) {
            terminal = true;
            info.set_solved();
            dead = !info.is_goal_state();
            // std::cout << this->state_id_map_->operator[](state) << " is
            // terminal (" << dead << ")" << std::endl;
            if (listener_ != nullptr && dead
                && !info.is_recognized_dead_end()) {
                info.set_recognized_dead_end();
                listener_->operator()(state);
            }
            push_parents_to_queue(info);
            backpropagate_tip_value();
        }

        assert(queue_.empty());
    }

    void push_parents_to_queue(StateInfo& info)
    {
        push_parents_to_queue(IsGreedy(), info);
    }

    void mark_solved_push_parents(
        const State& state,
        StateInfo& info,
        const bool dead)
    {
        mark_solved_push_parents(IsGreedy(), state, info, dead);
    }

private:
    struct QueueComparator {
        bool operator()(
            const std::pair<int, StateID>& s,
            const std::pair<int, StateID>& t) const
        {
            return s.first > t.first;
        }
    };

    class MarkDeadEnds;
    friend class MarkDeadEnds;
    class MarkDeadEnds {
    public:
        explicit MarkDeadEnds(AOBase* ao)
            : ao_(ao)
            , state_infos_(ao->state_infos_)
            , listener_(ao->listener_)
        {
        }

        void operator()(const State& s)
        {
            auto& info = state_infos_(s);
            assert(!info.is_solved());
            assert(!info.is_goal_state());
            assert(info.is_recognized_dead_end());
            info.set_solved();
            ao_->push_parents_to_queue(info);
            listener_->operator()(s);
        }

#if !defined(NDEBUG)
        template<typename T>
        void operator()(T begin, T end)
        {
            this->operator()(*begin);
            assert(++begin == end);
        }
#else
        template<typename T>
        void operator()(T begin, T)
        {
            this->operator()(*begin);
        }
#endif

    private:
        AOBase* ao_;
        StateStatusAccessor state_infos_;
        StateListener<State, Action>* listener_;
    };

    struct ExpansionCondition {
        explicit ExpansionCondition(
            StateStatusAccessor& state_infos,
            DeadEndIdentificationLevel level)
            : state_infos_(state_infos)
            , level_(level)
        {
        }

        bool operator()(const State& state)
        {
            const auto& state_info = state_infos_(state);
            if (state_info.is_solved()) {
                return true;
            }
            switch (level_) {
            case (DeadEndIdentificationLevel::Policy):
                assert(false);
                break;
            case (DeadEndIdentificationLevel::Visited):
                return !state_info.is_value_initialized();
            case (DeadEndIdentificationLevel::Complete):
                return false;
            default:
                break;
            }
            return false;
        }

        StateStatusAccessor state_infos_;
        const DeadEndIdentificationLevel level_;
    };

    using IsGreedy = std::integral_constant<bool, Greedy>;

    using TopoQueue = std::priority_queue<
        std::pair<int, StateID>,
        std::vector<std::pair<int, StateID>>,
        QueueComparator>;

    void push_parents_to_queue(const std::true_type&, StateInfo& info)
    {
        auto& parents = info.get_parents();
        for (auto it = parents.begin(); it != parents.end(); ++it) {
            auto& pinfo = state_infos_(*it);
            assert(!pinfo.is_dead_end() || pinfo.is_solved());
            if (pinfo.is_unflagged()) {
                pinfo.mark();
                queue_.emplace(pinfo.update_order, *it);
            }
        }
        if (info.is_solved()) {
            typename std::remove_reference<decltype(parents)>::type().swap(
                parents);
        }
    }

    void push_parents_to_queue(const std::false_type&, StateInfo& info)
    {
        auto& parents = info.get_parents();
        for (auto it = parents.begin(); it != parents.end(); ++it) {
            auto& pinfo = state_infos_(*it);
            assert(!pinfo.is_dead_end() || pinfo.is_solved());
            if (info.is_solved()) {
                assert(pinfo.unsolved > 0 || pinfo.is_solved());
                --pinfo.unsolved;
                if (!info.is_dead_end()) {
                    pinfo.alive = 1;
                }
            }
            if (pinfo.is_unflagged()) {
                pinfo.mark();
                queue_.emplace(pinfo.update_order, *it);
            }
        }
        if (info.is_solved()) {
            typename std::remove_reference<decltype(parents)>::type().swap(
                parents);
        }
    }

    bool update_value_check_solved(
        const std::true_type&,
        const State& state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
    {
        const bool result =
            this->async_update(state, nullptr, &selected_transition_);
        solved = true;
        dead = !selected_transition_.empty() || info.is_dead_end();
        // std::cout << "update " << this->state_id_map_->operator[](state) << "
        // ... [";
        for (auto it = selected_transition_.begin();
             it != selected_transition_.end();
             ++it) {
            // std::cout << " " << this->state_id_map_->operator[](it->first);
            const auto& succ_info = state_infos_(it->first);
            solved = solved && succ_info.is_solved();
            dead = dead && succ_info.is_dead_end();
        }
        // std::cout << " ] => " << solved << "|" << dead << std::endl;
        selected_transition_.clear();
        return result;
    }

    bool update_value_check_solved(
        const std::false_type&,
        const State& state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
    {
        solved = info.unsolved == 0;
        dead = solved && info.alive == 0 && !info.is_goal_state();
        return this->async_update(state);
    }

    void mark_solved_push_parents(
        const std::true_type&,
        const State& state,
        StateInfo& info,
        const bool dead)
    {
        assert(!info.is_terminal());
        if (dead && listener_ != nullptr) {
            std::pair<bool, bool> exploration_result;
            this->safe_async_update(
                state,
                mark_dead_ends_,
                expansion_condition_,
                exploration_result);
            assert(exploration_result.first || info.is_solved());
            if (exploration_result.first) {
                info.set_solved();
                push_parents_to_queue(info);
            }
        } else {
            info.set_solved();
            push_parents_to_queue(std::true_type(), info);
        }
    }

    void mark_solved_push_parents(
        const std::false_type&,
        const State& state,
        StateInfo& info,
        const bool dead)
    {
        assert(!info.is_terminal());
        if (dead && listener_ != nullptr) {
            info.set_recognized_dead_end();
            mark_dead_ends_(state);
        } else {
            info.set_solved();
            push_parents_to_queue(std::false_type(), info);
        }
    }

protected:
    StateStatusAccessor state_infos_;
    StateListener<State, Action>* listener_;
    const DeadEndIdentificationLevel dead_end_ident_level_;

    std::vector<Action> aops_;
    Distribution<State> selected_transition_;
    std::vector<StateID> successors_;

    internal::Statistics stats_;

private:
    TopoQueue queue_;
    MarkDeadEnds mark_dead_ends_;
    ExpansionCondition expansion_condition_;
};

} // namespace ao_search
} // namespace algorithms
} // namespace probabilistic
