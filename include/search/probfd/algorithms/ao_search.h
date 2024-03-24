#ifndef PROBFD_ALGORITHMS_AO_SEARCH_H
#define PROBFD_ALGORITHMS_AO_SEARCH_H

#include "probfd/algorithms/heuristic_search_base.h"

#include "probfd/storage/per_state_storage.h"

#include <iosfwd>
#include <queue>
#include <type_traits>
#include <vector>

/// Namespace dedicated to the AO* family of MDP algorithms.
namespace probfd::algorithms::ao_search {

struct Statistics {
    unsigned long long iterations = 0;
    void print(std::ostream& out) const;
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t MARK = 1 << StateInfo::BITS;
    static constexpr uint8_t SOLVED = 2 << StateInfo::BITS;
    static constexpr uint8_t MASK = 3 << StateInfo::BITS;
    static constexpr uint8_t BITS = StateInfo::BITS + 2;

    unsigned update_order = 0;
    std::vector<StateID> parents;

    [[nodiscard]]
    bool is_tip_state() const
    {
        return update_order == 0;
    }

    [[nodiscard]]
    bool is_marked() const
    {
        return this->info & MARK;
    }

    [[nodiscard]]
    bool is_solved() const
    {
        return this->info & SOLVED;
    }

    [[nodiscard]]
    bool is_unflagged() const
    {
        return (this->info & MASK) == 0;
    }

    [[nodiscard]]
    const std::vector<StateID>& get_parents() const
    {
        return parents;
    }

    [[nodiscard]]
    std::vector<StateID>& get_parents()
    {
        return parents;
    }

    void mark()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARK;
    }

    void unmark() { this->info = (this->info & ~MARK); }

    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }

    void add_parent(StateID s) { parents.push_back(s); }
};

/**
 * @brief Base class for the AO* algorithm family.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam Interval - Determines whether interval bounds are used.
 * @tparam StorePolicy - Determines whether a greedy policy is maintained.
 * @tparam StateInfoExtension - The extended state information struct used by
 * the derived algorithm.
 */
template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
class AOBase
    : public heuristic_search::HeuristicSearchAlgorithmExt<
          State,
          Action,
          Interval,
          StorePolicy,
          StateInfoExtension> {
    using Base = typename AOBase::HeuristicSearchAlgorithm;

protected:
    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;
    using PolicyPickerType = typename Base::PolicyPicker;
    using StateInfo = typename Base::StateInfo;

private:
    struct PrioritizedStateID {
        unsigned update_order;
        StateID state_id;

        friend bool operator<(
            const PrioritizedStateID& left,
            const PrioritizedStateID& right)
        {
            return left.update_order > right.update_order;
        }
    };

    std::priority_queue<PrioritizedStateID> queue_;

protected:
    std::vector<Transition<Action>> transitions_;

    Statistics statistics_;

public:
    // Inherit constructor
    using Base::Base;

protected:
    void print_additional_statistics(std::ostream& out) const override;

    void
    setup_custom_reports(param_type<State>, ProgressReport& progress) override;

    void backpropagate_tip_value(
        MDPType& mdp,
        EvaluatorType& heuristic,
        utils::CountdownTimer& timer);

    void backpropagate_update_order(StateID tip, utils::CountdownTimer& timer);

    void initialize_tip_state_value(
        MDPType& mdp,
        EvaluatorType& heuristic,
        StateID state,
        StateInfo& info,
        bool& terminal,
        bool& solved,
        bool& dead,
        bool& value_changed,
        utils::CountdownTimer& timer);

    void push_parents_to_queue(StateInfo& info);

    void mark_solved_push_parents(StateInfo& info, bool dead);

private:
    bool update_value_check_solved(
        MDPType& mdp,
        EvaluatorType& heuristic,
        StateID state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
        requires(StorePolicy);

    bool update_value_check_solved(
        MDPType& mdp,
        EvaluatorType& heuristic,
        StateID state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
        requires(!StorePolicy);
};

} // namespace probfd::algorithms::ao_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_SEARCH_H
#include "probfd/algorithms/ao_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_SEARCH_H

#endif // PROBFD_ALGORITHMS_AO_SEARCH_H