#ifndef PROBFD_QUOTIENT_SYSTEM_H
#define PROBFD_QUOTIENT_SYSTEM_H

#include "algorithms/segmented_vector.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/task_state_space.h"

#include "operator_id.h"
#include "task_proxy.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ranges>
#include <set>
#include <unordered_set>
#include <vector>

namespace probfd {
namespace quotients {

/**
 * FIXME: The below specialization of a task-level quotient state space is
 * supposed to cache the quotient's transitions to speed up the successor
 * generation. However, is has several issues:
 *
 * 1. The object uses the cache of the original task state space implementation,
 * and updates it whenever a quotient is built. This is a nasty hack and
 * invalidates the original transition system. The implementation should have
 * its own independent transition cache and there should not be a dependency
 * to a particular implementation of the original transition system.
 * 2. On top of this, the cache update is currently broken, leading to incorrect
 * transitions being generated.
 * 3. The implementation does not the store the pruned part of the quotient,
 * but this information is desperately needed during policy extraction, as the
 * policy is unspecified with respect to states within a quotient when a
 * trap-collapsing algorithm finishes, and reconstructing this information
 * requires access to the pruned transition.
 */

/*
template <>
class QuotientSystem<State, OperatorID>
    : public engine_interfaces::
          StateSpace<State, quotients::QuotientAction<OperatorID>> {
    friend struct const_iterator;

    struct QuotientInformation {
        std::vector<StateID> parents;
        std::vector<StateID> states;
        explicit QuotientInformation(const StateID& s)
            : states({s})
        {
        }
    };

    const bool cache_;

    segmented_vector::SegmentedVector<QuotientInformation> state_infos_;
    engine_interfaces::StateSpace<State, OperatorID>* state_space_;

    std::unique_ptr<DefaultQuotientSystem<State, OperatorID>> fallback_;

public:
    using QAction = QuotientAction<OperatorID>;
    using QuotientStateIDIterator =
        DefaultQuotientSystem<State, OperatorID>::QuotientStateIDIterator;

    class const_iterator {
        const QuotientSystem* qs_;
        DefaultQuotientSystem<State, OperatorID>::const_iterator i;

    public:
        using value_type = StateID;
        using difference_type = std::ptrdiff_t;

        const_iterator(
            const QuotientSystem* qs,
            DefaultQuotientSystem<State, OperatorID>::const_iterator x);

        const_iterator& operator++();
        const_iterator operator++(int);

        friend bool operator==(const const_iterator&, const const_iterator&);

        StateID operator*() const;
    };

    static_assert(std::input_iterator<const_iterator>);

    explicit QuotientSystem(
        engine_interfaces::StateSpace<State, OperatorID>* state_space);

    StateID get_state_id(param_type<State> s) override;
    State get_state(StateID sid) override;

    QAction get_action(StateID sid, ActionID aid) override;
    ActionID get_action_id(StateID sid, param_type<QAction> a) override;

    void generate_applicable_actions(StateID sid, std::vector<QAction>& result)
        override;

    void generate_action_transitions(
        StateID,
        param_type<QAction> a,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateID sid,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    unsigned quotient_size(StateID state_id) const;

    engine_interfaces::StateSpace<State, OperatorID>* get_parent_state_space();

    const_iterator begin() const;
    const_iterator end() const;

    std::ranges::subrange<QuotientStateIDIterator, QuotientStateIDIterator>
    quotient_range(const StateID& state_id) const;

    StateID translate_state_id(StateID sid) const;

    void get_pruned_ops(StateID sid, std::vector<QAction>& result);
    
    OperatorID get_original_action(StateID, const QAction& a) const;
    ActionID get_original_action_id(StateID sid, ActionID a) const;

    template <std::ranges::input_range StateIDRange>
    void build_quotient(StateIDRange&& range)
        requires(
            std::is_same_v<std::ranges::range_value_t<StateIDRange>, StateID>)
    {
        this->build_quotient(
            std::ranges::begin(range),
            std::ranges::end(range));
    }

    template <std::input_iterator StateIDIterator>
    void build_quotient(StateIDIterator begin, StateIDIterator end)
        requires(std::is_same_v<std::iter_value_t<StateIDIterator>, StateID>)
    {
        if (begin != end) {
            this->build_quotient(begin, end, *begin);
        }
    }

    template <std::input_iterator StateIDIterator>
    void build_quotient(StateIDIterator begin, StateIDIterator end, StateID rid)
        requires(std::is_same_v<std::iter_value_t<StateIDIterator>, StateID>)
    {
        this->build_quotient(
            begin,
            end,
            rid,
            utils::infinite_iterator<std::vector<OperatorID>>());
    }

    template <
        std::input_iterator StateIDIterator,
        std::input_iterator ActionFilterIterator>
    void build_quotient(
        StateIDIterator begin,
        StateIDIterator end,
        StateID rid,
        ActionFilterIterator filter_it)
        requires(
            std::is_same_v<std::iter_value_t<StateIDIterator>, StateID> &&
            std::is_same_v<
                std::iter_value_t<ActionFilterIterator>,
                std::vector<OperatorID>>)
    {
        assert(utils::contains(std::ranges::subrange(begin, end), rid));

        if (!cache_) {
            fallback_->build_quotient(begin, end, rid, filter_it);
            return;
        }

        QuotientInformation& dest_info = state_infos_[rid];
        std::vector<StateID>& parents = dest_info.parents;
        std::vector<StateID>& states = dest_info.states;
        const unsigned old_qstates = states.size();

        std::unordered_set<StateID> states_set;

        for (auto it = begin; it != end; ++it) {
            const StateID state_id = *it;

            if (state_id == rid) {
                continue;
            }

            QuotientInformation& info = state_infos_[state_id];
            assert(info.states[0] == state_id);

            std::ranges::move(info.parents, std::back_inserter(parents));

            // Release Memory
            decltype(info.parents)().swap(info.parents);

            states.insert(states.end(), info.states.begin(), info.states.end());

            assert(utils::contains(states, state_id));
        }

        states_set.insert(states.begin(), states.end());

        for (auto it = begin; it != end; ++it, ++filter_it) {
            const StateID state_id = *it;
            QuotientInformation& info = state_infos_[state_id];
            const int e = (state_id != rid ? info.states.size() : old_qstates);

            for (int i = 0; i < e; ++i) {
                auto state = info.states[i];
                update_cache(
                    *filter_it,
                    state_space_->lookup(state),
                    rid,
                    states_set);
                state_infos_[state].states[0] = rid;
            }

            if (state_id != rid) {
                info.states.resize(1);
                info.states[0] = rid;
                info.states.shrink_to_fit();
            }
        }

        const auto [first, last] =
            std::ranges::remove_if(parents, [rid, this](StateID parent) {
                return state_infos_[parent].states[0] != rid;
            });

        parents.erase(first, last);
        utils::sort_unique(parents);
        parents.shrink_to_fit();

        const ProbabilisticOperatorsProxy operators =
            state_space_->task_proxy.get_operators();

        for (StateID parent : parents) {
            assert(parent != rid);
            assert(state_infos_[parent].states[0] == parent);

            const auto& parent_states = state_infos_[parent].states;

            for (StateID parent_state : parent_states) {
                auto& entry = state_space_->lookup(parent_state);
                const OperatorID* aop = entry.aops;
                const OperatorID* aop_end = entry.aops + entry.naops;
                StateID* succ = entry.succs;

                for (; aop != aop_end; ++aop) {
                    auto succ_e = succ + operators[*aop].get_outcomes().size();
                    for (; succ != succ_e; ++succ) {
                        if (states_set.contains(*succ)) {
                            *succ = rid;
                        }
                        // *succ = state_infos_[*succ].states[0];
                        assert(state_infos_[*succ].states[0] == *succ);
                    }
                }
            }
        }

        assert(!state_infos_[rid].states.empty());
        assert(state_infos_[rid].states[0] == rid);
        assert(!utils::contains(state_infos_[rid].parents, rid));
        assert(utils::is_unique(state_infos_[rid].states));
        assert(utils::is_unique(state_infos_[rid].parents));

#ifndef NDEBUG
        verify_cache_consistency();
#endif
    }

private:
    const QuotientInformation* get_infos(StateID sid) const;

    engine_interfaces::StateSpace<State, OperatorID>::CacheEntry&
    lookup(StateID sid);

    const engine_interfaces::StateSpace<State, OperatorID>::CacheEntry&
    lookup(StateID sid) const;

    void update_cache(
        const std::vector<OperatorID>& exclude,
        engine_interfaces::StateSpace<State, OperatorID>::CacheEntry& entry,
        const StateID rid,
        const std::unordered_set<StateID>& quotient_states);

#ifndef NDEBUG
    void verify_cache_consistency();
#endif
};

*/

} // namespace quotients
} // namespace probfd

#endif // __QUOTIENT_SYSTEM_H__