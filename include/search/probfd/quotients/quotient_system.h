#ifndef PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
#define PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H

#include "probfd/engine_interfaces/state_space.h"

#include "algorithms/segmented_vector.h"

#include "utils/collections.h"

#include "task_proxy.h"

#include <deque>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class OperatorID;

namespace probfd {
namespace quotients {

template <typename Action>
struct QuotientAction {
    StateID state_id;
    ActionID action_id;
};

template <typename State, typename Action>
class DefaultQuotientSystem {
    friend struct const_iterator;

    struct QuotientInformation {
        std::vector<std::pair<StateID, unsigned>> states_naops;
        std::vector<ActionID> aops;

        using iterator = decltype(states_naops.begin());
        using const_iterator = decltype(std::as_const(states_naops).begin());

        using state_iterator = utils::key_iterator<iterator>;
        using const_state_iterator = utils::key_iterator<const_iterator>;

        auto begin() { return states_naops.begin(); }
        auto end() { return states_naops.end(); }

        auto begin() const { return states_naops.begin(); }
        auto end() const { return states_naops.end(); }

        auto state_begin() { return utils::make_key_iterator(begin()); }
        auto state_end() { return utils::make_key_iterator(end()); }

        auto state_begin() const { return utils::make_key_iterator(begin()); }
        auto state_end() const { return utils::make_key_iterator(end()); }
    };

    std::unordered_map<StateID::size_type, QuotientInformation> quotients_;
    segmented_vector::SegmentedVector<StateID::size_type> quotient_ids_;
    engine_interfaces::StateSpace<State, Action>* state_space_;

    // MASK: bitmask used to obtain the quotient state id, if it exists
    // FLAG: whether a quotient state id exists
    static constexpr StateID::size_type MASK = (StateID::size_type(-1) >> 1);
    static constexpr StateID::size_type FLAG = ~MASK;

public:
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator = utils::variant_iterator<
        typename QuotientInformation::const_state_iterator,
        const StateID*>;

    struct const_iterator {
        const DefaultQuotientSystem* qs_;
        StateID i;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = StateID;

        const_iterator(const DefaultQuotientSystem* qs, StateID x)
            : qs_(qs)
            , i(x)

        {
        }

        const_iterator& operator++()
        {
            while (++i.id < qs_->quotient_ids_.size()) {
                const StateID ref = qs_->quotient_ids_[i];
                if (i == (ref & DefaultQuotientSystem::MASK)) {
                    break;
                }
            }

            return *this;
        }

        const_iterator operator++(int)
        {
            auto r = *this;
            ++(*this);
            return r;
        }

        friend bool
        operator==(const const_iterator& left, const const_iterator& right)
        {
            return left.i == right.i;
        }

        StateID operator*() const { return i; }
    };

    static_assert(std::input_iterator<const_iterator>);

    explicit DefaultQuotientSystem(
        engine_interfaces::StateSpace<State, Action>* state_space)
        : state_space_(state_space)
    {
    }

    const_iterator begin() const { return const_iterator(this, 0); }

    const_iterator end() const
    {
        return const_iterator(this, quotient_ids_.size());
    }

    unsigned quotient_size(StateID state_id) const
    {
        const QuotientInformation* info = get_quotient_info(state_id);
        return info ? info->states_naops.size() : 1;
    }

    auto quotient_range(const StateID& state_id) const
    {
        const QuotientInformation* info = this->get_quotient_info(state_id);

        if (info) {
            return std::ranges::subrange(
                QuotientStateIDIterator(info->state_begin()),
                QuotientStateIDIterator(info->state_end()));
        }

        return std::ranges::subrange(
            QuotientStateIDIterator(&state_id),
            QuotientStateIDIterator(&state_id + 1));
    }

    void
    generate_applicable_ops(StateID sid, std::vector<QAction>& result) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig;
            state_space_->generate_applicable_actions(sid, orig);

            result.reserve(orig.size());

            for (const Action& a : orig) {
                result.emplace_back(sid, state_space_->get_action_id(sid, a));
            }
        } else {
            result.reserve(info->aops.size());

            auto aid = info->aops.begin();

            for (const auto& [qsid, naops] : info->states_naops) {
                for (auto aops_end = aid + naops; aid != aops_end; ++aid) {
                    result.emplace_back(qsid, *aid);
                }
            }
        }
    }

    void generate_successors(
        StateID,
        const QAction& a,
        Distribution<StateID>& result) const
    {
        const auto act = state_space_->get_action(a.state_id, a.action_id);

        Distribution<StateID> orig;
        state_space_->generate_action_transitions(a.state_id, act, orig);

        for (const auto& [state_id, probability] : orig) {
            result.add(get_masked_state_id(state_id) & MASK, probability);
        }
    }

    void generate_all_successors(
        StateID sid,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig_a;
            state_space_->generate_applicable_actions(sid, orig_a);

            aops.reserve(orig_a.size());
            successors.resize(orig_a.size());

            for (unsigned i = 0; i < orig_a.size(); ++i) {
                ActionID aid = state_space_->get_action_id(sid, orig_a[i]);
                const QAction& a = aops.emplace_back(sid, aid);
                generate_successors(sid, a, successors[i]);
            }
        } else {
            aops.reserve(info->aops.size());
            successors.resize(info->aops.size());

            auto aop = info->aops.begin();
            unsigned k = 0;

            for (const auto& [qsid, naops] : info->states_naops) {
                for (auto aops_end = aop + naops; aop != aops_end; ++aop, ++k) {
                    const QAction& a = aops.emplace_back(qsid, *aop);
                    generate_successors(sid, a, successors[k]);
                }
            }
        }
    }

    StateID get_state_id(const State& s) const
    {
        return state_space_->get_state_id(s);
    }

    State get_state(StateID sid) const { return state_space_->get_state(sid); }

    StateID translate_state_id(StateID sid) const
    {
        return StateID(get_masked_state_id(sid) & MASK);
    }

    ActionID get_original_action_id(StateID sid, ActionID a) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        return info ? info->aops[a] : a;
    }

    ActionID get_action_id(StateID sid, const QAction& a) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            return a.action_id;
        }

        auto state_naops = info->states_naops.begin();
        auto aop = info->aops.begin();

        while (state_naops->first != a.state_id) {
            aop += state_naops->second;
            ++state_naops;
        }

        while (*aop != a.action_id) {
            ++aop;
        }

        return aop - info->aops.begin();
    }

    Action get_original_action(StateID, const QAction& a) const
    {
        return state_space_->get_action(a.state_id, a.action_id);
    }

    QAction get_action(StateID sid, ActionID aid) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            return QAction(sid, aid);
        }

        auto qsid_naops = info->states_naops.begin();
        unsigned sum = qsid_naops->second;

        while (sum <= aid) {
            ++qsid_naops;
            sum += qsid_naops->second;
        }

        return QAction(qsid_naops->first, info->aops[aid]);
    }

    template <typename Range>
    void build_quotient(Range& range)
    {
        this->build_quotient(range.begin(), range.end());
    }

    template <typename StateIDIterator>
    void build_quotient(StateIDIterator begin, StateIDIterator end)
    {
        if (begin != end) {
            this->build_quotient(begin, end, *begin);
        }
    }

    template <typename StateIDIterator>
    void build_quotient(StateIDIterator begin, StateIDIterator end, StateID rid)
    {
        this->build_quotient(
            begin,
            end,
            rid,
            utils::infinite_iterator<std::vector<Action>>());
    }

    template <typename StateIDIterator, typename IgnoreActionsIterator>
    void build_quotient(
        StateIDIterator begin,
        StateIDIterator end,
        StateID rid, // representative id
        IgnoreActionsIterator ignore_actions)
    {
        // Get or create quotient
        QuotientInformation& qinfo = quotients_[rid];

        auto rit = std::find(begin, end, rid);
        auto ridx = std::distance(begin, rit);

        // We handle the representative state first so that it
        // appears first in the data structure.
        if (qinfo.states_naops.empty()) {
            // Add this state to the quotient
            auto& b = qinfo.states_naops.emplace_back(rid, 0);
            set_masked_state_id(rid, rid);

            // Generate the applicable actions
            std::vector<Action> gen_aops;
            state_space_->generate_applicable_actions(rid, gen_aops);

            // Filter actions
            filter_actions(gen_aops, ignore_actions[ridx]);

            // Add the action ids to the new quotient
            for (const Action& a : gen_aops) {
                ActionID aid = state_space_->get_action_id(rid, a);
                qinfo.aops.push_back(aid);
            }

            b.second = gen_aops.size();
        } else {
            // Filter actions
            filter_actions(
                qinfo.states_naops,
                qinfo.aops,
                ignore_actions[ridx]);
        }

        for (auto it = begin; it != end; ++it, ++ignore_actions) {
            const auto& state_id = *it;

            // Already handled.
            if (state_id == rid) {
                continue;
            }

            const StateID::size_type qsqid = get_masked_state_id(state_id);

            // If the state is a quotient state, add all states it
            // represents to the new quotient
            if (qsqid & FLAG) {
                // Get the old quotient
                auto qit = quotients_.find(qsqid & MASK);
                QuotientInformation& q = qit->second;

                // Filter actions
                filter_actions(q.states_naops, q.aops, *ignore_actions);

                // Insert all states belonging to it to the new quotient
                for (const auto& p : q.states_naops) {
                    qinfo.states_naops.push_back(p);
                    set_masked_state_id(p.first, rid);
                }

                // Move the actions to the new quotient
                std::ranges::move(q.aops, std::back_inserter(qinfo.aops));

                // Erase the old quotient
                quotients_.erase(qit);
            } else {
                // Add this state to the quotient
                auto& b = qinfo.states_naops.emplace_back(state_id, 0);
                set_masked_state_id(state_id, rid);

                // Generate the applicable actions
                std::vector<Action> gen_aops;
                state_space_->generate_applicable_actions(state_id, gen_aops);

                // Filter actions
                filter_actions(gen_aops, *ignore_actions);

                // Add the action ids to the new quotient
                for (const Action& a : gen_aops) {
                    ActionID aid = state_space_->get_action_id(state_id, a);
                    qinfo.aops.push_back(aid);
                }

                b.second = gen_aops.size();
            }
        }
    }

private:
    inline void
    filter_actions(std::vector<Action>& aops, const std::vector<Action>& filter)
        const
    {
        if (filter.empty()) {
            return;
        }

        auto rem = [&filter](const Action& a) {
            return utils::contains(filter, a);
        };

        aops.erase(std::remove_if(aops.begin(), aops.end(), rem), aops.end());
    }

    inline void filter_actions(
        std::vector<std::pair<StateID, unsigned>>& states_naops,
        std::vector<ActionID>& aops,
        const std::vector<Action>& filter) const
    {
        if (filter.empty()) {
            return;
        }

        // Immitate std::remove_if
        auto it = aops.end();
        auto aopsit = aops.begin();

        for (auto& [qsid, naops] : states_naops) {
            auto aops_end = aopsit + naops;
            for (aopsit = it; aopsit != aops_end; ++aopsit) {
                Action a = state_space_->get_action(qsid, *aopsit);
                if (!utils::contains(filter, a) && it != aops_end) {
                    *it++ = std::move(*aopsit);
                } else {
                    if (it == aops_end) {
                        it = aopsit;
                    }
                    --naops;
                }
            }
        }

        aops.erase(it, aops.end());
    }

    QuotientInformation* get_quotient_info(StateID state_id)
    {
        const StateID::size_type qid = quotient_ids_[state_id];
        return qid & FLAG ? &quotients_.find(qid & MASK)->second : nullptr;
    }

    const QuotientInformation* get_quotient_info(StateID state_id) const
    {
        const StateID::size_type qid = get_masked_state_id(state_id);
        return qid & FLAG ? &quotients_.find(qid & MASK)->second : nullptr;
    }

    StateID::size_type get_masked_state_id(StateID sid) const
    {
        return sid < quotient_ids_.size() ? quotient_ids_[sid] : sid.id;
    }

    void set_masked_state_id(StateID sid, const StateID::size_type& qsid)
    {
        if (sid >= quotient_ids_.size()) {
            for (auto idx = quotient_ids_.size(); idx <= sid; ++idx) {
                quotient_ids_.push_back(idx);
            }
        }

        quotient_ids_[sid] = qsid | FLAG;
    }
};

template <typename State, typename Action>
class QuotientSystem : public DefaultQuotientSystem<State, Action> {
public:
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator =
        typename DefaultQuotientSystem<State, Action>::QuotientStateIDIterator;
    using DefaultQuotientSystem<State, Action>::DefaultQuotientSystem;
};

template <typename State>
class QuotientSystem<State, OperatorID>;

} // namespace quotients
} // namespace probfd

#endif // __QUOTIENT_SYSTEM_H__
