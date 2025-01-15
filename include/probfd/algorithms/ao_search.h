#ifndef ALGORITHMS_AO_SEARCH_H
#define ALGORITHMS_AO_SEARCH_H

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

template <typename Action, bool Interval, bool StorePolicy>
struct PerStateInformation
    : public heuristic_search::
          PerStateBaseInformation<Action, StorePolicy, Interval> {
private:
    using Base = heuristic_search::PerStateBaseInformation<Action, StorePolicy, Interval>;

public:
    static constexpr uint8_t MARK = 1 << Base::BITS;
    static constexpr uint8_t SOLVED = 2 << Base::BITS;
    static constexpr uint8_t MASK = 3 << Base::BITS;
    static constexpr uint8_t BITS = Base::BITS + 2;

    unsigned update_order = 0;
    std::vector<StateID> parents;

    [[nodiscard]]
    bool is_marked() const
    {
        return this->info & MARK;
    }

    [[nodiscard]]
    bool is_solved() const
    {
        return this->info & SOLVED || this->is_goal_or_terminal();
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
 * @tparam StateInfo - The state information struct used by the derived
 * algorithm.
 */
template <typename State, typename Action, typename StateInfo>
class AOBase
    : public heuristic_search::
          HeuristicSearchAlgorithm<State, Action, StateInfo> {
    using Base = typename AOBase::HeuristicSearchAlgorithm;

protected:
    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;
    using PolicyPickerType = typename Base::PolicyPicker;

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
    Statistics statistics_;

public:
    // Inherit constructor
    using Base::Base;

protected:
    void print_additional_statistics(std::ostream& out) const override;

    void backpropagate_tip_value(
        this auto& self,
        MDPType& mdp,
        std::vector<Transition<Action>>& transitions,
        StateInfo& state_info,
        utils::CountdownTimer& timer);

    void backpropagate_update_order(
        StateID tip,
        StateInfo& info,
        unsigned update_order,
        utils::CountdownTimer& timer);

private:
    void push_parents_to_queue(StateInfo& info);
};

} // namespace probfd::algorithms::ao_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_SEARCH_H
#include "probfd/algorithms/ao_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_SEARCH_H

#endif // ALGORITHMS_AO_SEARCH_H
