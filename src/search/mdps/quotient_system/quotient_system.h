#ifndef MDPS_QUOTIENT_SYSTEM_QUOTIENT_SYSTEM_H
#define MDPS_QUOTIENT_SYSTEM_QUOTIENT_SYSTEM_H

#include "../../algorithms/segmented_vector.h"
#include "../../utils/collections.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/transition_generator.h"

#include <deque>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace probabilistic {

/// Namespace dedicated to the classes dealing with quotienting of an MDP.
namespace quotient_system {

template <typename Action>
struct QuotientAction {
    StateID state_id;
    ActionID action_id;

    explicit QuotientAction(const StateID& state_id, const ActionID& action_id)
        : state_id(state_id)
        , action_id(action_id)
    {
    }
};

template <typename BaseActionT>
class DefaultQuotientSystem {
    friend struct const_iterator;

    struct QuotientInformation {
        std::vector<std::pair<StateID, unsigned>> states_naops;
        std::vector<ActionID> aops;

        using iterator = decltype(states_naops.begin());
        using const_iterator = decltype(std::as_const(states_naops).begin());

        using state_iterator = utils::key_iterator<iterator>;
        using const_state_iterator = utils::const_key_iterator<const_iterator>;

        iterator begin() { return states_naops.begin(); }
        iterator end() { return states_naops.end(); }

        const_iterator begin() const { return states_naops.begin(); }
        const_iterator end() const { return states_naops.end(); }

        state_iterator state_begin() { return state_iterator(begin()); }
        state_iterator state_end() { return state_iterator(end()); }

        const_state_iterator state_begin() const
        {
            return const_state_iterator(begin());
        }
        const_state_iterator state_end() const
        {
            return const_state_iterator(end());
        }
    };

    using QuotientMap =
        std::unordered_map<StateID::size_type, QuotientInformation>;

public:
    using Action = BaseActionT;
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator = utils::common_iterator<
        typename QuotientInformation::const_state_iterator,
        const StateID*>;

    // MASK: bitmask used to obtain the quotient state id, if it exists
    // FLAG: whether a quotient state id exists
    static constexpr StateID::size_type MASK = (StateID::size_type(-1) >> 1);
    static constexpr StateID::size_type FLAG = ~MASK;

    struct const_iterator {
        using iterator_type = std::forward_iterator_tag;
        using value_type = StateID::size_type;
        using difference_type = int;
        using pointer = const StateID::size_type*;
        using reference = const StateID::size_type&;

        explicit const_iterator(
            const DefaultQuotientSystem* qs,
            StateID::size_type x)
            : i(x)
            , qs_(qs)
        {
        }

        const_iterator& operator++()
        {
            while (++i < qs_->quotient_ids_.size()) {
                const StateID::size_type& ref = qs_->quotient_ids_[i];
                if (i == (ref & DefaultQuotientSystem::MASK)) {
                    break;
                }
            }

            return *this;
        }

        const_iterator operator+=(int x)
        {
            const_iterator res = *this;
            while (x-- > 0)
                this->operator++();
            return res;
        }

        bool operator==(const const_iterator& other) const
        {
            return i == other.i;
        }

        bool operator!=(const const_iterator& other) const
        {
            return i != other.i;
        }

        reference operator*() const { return i; }

        StateID::size_type i;

    private:
        const DefaultQuotientSystem* qs_;
    };

    explicit DefaultQuotientSystem(
        ActionIDMap<Action>* action_id_map,
        ApplicableActionsGenerator<Action>* aops_gen,
        TransitionGenerator<Action>* transition_gen)
        : quotients_()
        , quotient_ids_()
        , action_id_map_(action_id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
    {
    }

    const_iterator begin() const { return const_iterator(this, 0); }

    const_iterator end() const
    {
        return const_iterator(this, quotient_ids_.size());
    }

    unsigned quotient_size(const StateID& state_id) const
    {
        const QuotientInformation* info = get_quotient_info(state_id);
        return info ? info->states_naops.size() : 1;
    }

    utils::RangeProxy<QuotientStateIDIterator>
    quotient_range(const StateID& state_id) const
    {
        const QuotientInformation* info = this->get_quotient_info(state_id);

        if (info) {
            return utils::RangeProxy<QuotientStateIDIterator>(
                QuotientStateIDIterator(info->state_begin()),
                QuotientStateIDIterator(info->state_end()));
        }

        return utils::RangeProxy<QuotientStateIDIterator>(
            QuotientStateIDIterator(&state_id),
            QuotientStateIDIterator(&state_id + 1));
    }

    void
    generate_applicable_ops(const StateID& sid, std::vector<QAction>& result)
        const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig;
            this->aops_gen_->operator()(sid, orig);

            result.reserve(orig.size());

            for (const Action& a : orig) {
                result.emplace_back(sid, action_id_map_->get_action_id(sid, a));
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
        const StateID&,
        const QAction& a,
        Distribution<StateID>& result) const
    {
        const auto act = action_id_map_->get_action(a.state_id, a.action_id);

        Distribution<StateID> orig;
        this->transition_gen_->operator()(a.state_id, act, orig);

        for (const auto& [state_id, probability] : orig) {
            result.add(get_masked_state_id(state_id) & MASK, probability);
        }
    }

    void generate_all_successors(
        const StateID& sid,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig_a;
            this->aops_gen_->operator()(sid, orig_a);

            aops.reserve(orig_a.size());
            successors.resize(orig_a.size());

            for (unsigned i = 0; i < orig_a.size(); ++i) {
                ActionID aid = action_id_map_->get_action_id(sid, orig_a[i]);
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

    StateID translate_state_id(const StateID& sid) const
    {
        return StateID(get_masked_state_id(sid) & MASK);
    }

    ActionID get_original_action_id(const StateID& sid, const ActionID& a) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        return info ? info->aops[a] : a;
    }

    ActionID get_action_id(const StateID& sid, const QAction& a) const
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

    Action get_original_action(const StateID&, const QAction& a) const
    {
        return action_id_map_->get_action(a.state_id, a.action_id);
    }

    QAction get_action(const StateID& sid, const ActionID& aid) const
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

    template <typename StateIDIterator>
    void build_quotient(
        StateIDIterator begin,
        StateIDIterator end,
        const StateID& rid)
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
        const StateID& rid, // representative id
        IgnoreActionsIterator ignore_actions)
    {
        // Get or create quotient
        QuotientInformation& qinfo = quotients_[rid];

        auto rit = std::find(begin, end, rid);
        auto ridx = rit - begin;
        assert(begin + ridx != end);

        // We handle the representative state first so that it
        // appears first in the data structure.
        if (qinfo.states_naops.empty()) {
            // Add this state to the quotient
            auto& b = qinfo.states_naops.emplace_back(rid, 0);
            set_masked_state_id(rid, rid);

            // Generate the applicable actions
            std::vector<Action> gen_aops;
            aops_gen_->operator()(rid, gen_aops);

            // Filter actions
            filter_actions(gen_aops, ignore_actions[ridx]);

            // Add the action ids to the new quotient
            for (const Action& a : gen_aops) {
                ActionID aid = action_id_map_->get_action_id(rid, a);
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
                qinfo.aops.insert(
                    qinfo.aops.end(),
                    q.aops.begin(),
                    q.aops.end());

                // Erase the old quotient
                quotients_.erase(qit);
            } else {
                // Add this state to the quotient
                auto& b = qinfo.states_naops.emplace_back(state_id, 0);
                set_masked_state_id(state_id, rid);

                // Generate the applicable actions
                std::vector<Action> gen_aops;
                aops_gen_->operator()(state_id, gen_aops);

                // Filter actions
                filter_actions(gen_aops, *ignore_actions);

                // Add the action ids to the new quotient
                for (const Action& a : gen_aops) {
                    ActionID aid = action_id_map_->get_action_id(state_id, a);
                    qinfo.aops.push_back(aid);
                }

                b.second = gen_aops.size();
            }
        }
    }

    ActionIDMap<Action>* get_action_id_map() const { return action_id_map_; }

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
                Action a = action_id_map_->get_action(qsid, *aopsit);
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

    QuotientInformation* get_quotient_info(const StateID& state_id)
    {
        const StateID::size_type qid = quotient_ids_[state_id];
        return qid & FLAG ? &quotients_.find(qid & MASK)->second : nullptr;
    }

    const QuotientInformation* get_quotient_info(const StateID& state_id) const
    {
        const StateID::size_type qid = get_masked_state_id(state_id);
        return qid & FLAG ? &quotients_.find(qid & MASK)->second : nullptr;
    }

    StateID::size_type get_masked_state_id(const StateID& sid) const
    {
        return sid < quotient_ids_.size() ? quotient_ids_[sid] : sid;
    }

    void set_masked_state_id(const StateID& sid, const StateID::size_type& qsid)
    {
        if (sid >= quotient_ids_.size()) {
            for (auto idx = quotient_ids_.size(); idx <= sid; ++idx) {
                quotient_ids_.push_back(idx);
            }
        }

        quotient_ids_[sid] = qsid | FLAG;
    }

    QuotientMap quotients_;
    segmented_vector::SegmentedVector<StateID::size_type> quotient_ids_;
    ActionIDMap<Action>* action_id_map_;
    ApplicableActionsGenerator<Action>* aops_gen_;
    TransitionGenerator<Action>* transition_gen_;
};

template <typename ActionT>
class QuotientSystem : public DefaultQuotientSystem<ActionT> {
public:
    using Action = ActionT;
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator =
        typename DefaultQuotientSystem<Action>::QuotientStateIDIterator;
    using DefaultQuotientSystem<Action>::DefaultQuotientSystem;
};

} // namespace quotient_system
} // namespace probabilistic

#endif // __QUOTIENT_SYSTEM_H__