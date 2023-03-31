#ifndef PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
#define PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H

#include "probfd/engine_interfaces/state_space.h"

#include "algorithms/segmented_vector.h"

#include "utils/collections.h"

#include "task_proxy.h"

#include <compare>
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

    friend auto
    operator<=>(const QuotientAction&, const QuotientAction&) = default;
};

template <typename State, typename Action>
class DefaultQuotientSystem
    : public engine_interfaces::
          StateSpace<State, quotients::QuotientAction<Action>> {
    friend struct const_iterator;

    struct QuotientInformation {
        struct StateInfo {
            StateID state_id;
            size_t num_outer_acts = 0;
            size_t num_inner_acts = 0;
        };

        std::vector<StateInfo> state_infos;
        std::vector<ActionID> aops; // First outer, then inner actions
        size_t total_num_outer_acts = 0;

        auto begin() { return state_infos.begin(); }
        auto end() { return state_infos.end(); }

        auto begin() const { return state_infos.begin(); }
        auto end() const { return state_infos.end(); }

        auto state_begin()
        {
            return utils::make_transform_iterator(
                begin(),
                &StateInfo::state_id);
        }

        auto state_end()
        {
            return utils::make_transform_iterator(end(), &StateInfo::state_id);
        }

        auto state_begin() const
        {
            return utils::make_transform_iterator(
                begin(),
                &StateInfo::state_id);
        }

        auto state_end() const
        {
            return utils::make_transform_iterator(end(), &StateInfo::state_id);
        }

        void filter_actions(
            engine_interfaces::StateSpace<State, Action>& state_space,
            const std::vector<QuotientAction<Action>>& filter)
        {
            if (filter.empty()) {
                return;
            }

            total_num_outer_acts = 0;

            auto act_it = aops.begin();

            for (auto& info : state_infos) {
                auto outer_end = std::stable_partition(
                    act_it,
                    act_it + info.num_outer_acts,
                    [&info, &filter, &state_space](ActionID aid) {
                        return !utils::contains(
                            filter,
                            QuotientAction<Action>(info.state_id, aid));
                    });

                const size_t num_total_acts =
                    info.num_outer_acts + info.num_inner_acts;

                info.num_outer_acts = std::distance(act_it, outer_end);
                info.num_inner_acts = num_total_acts - info.num_outer_acts;

                total_num_outer_acts += info.num_outer_acts;

                act_it += num_total_acts;
            }

            assert(act_it == aops.end());
        }
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
        decltype(std::declval<const QuotientInformation>().state_begin()),
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

    StateID get_state_id(param_type<State> s) override
    {
        return state_space_->get_state_id(s);
    }

    State get_state(StateID sid) override
    {
        return state_space_->get_state(sid);
    }

    ActionID get_action_id(StateID sid, QAction a) override
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            return a.action_id;
        }

        auto state_info_it = info->state_infos.begin();
        auto op_it = info->aops.begin();

        while (state_info_it->state_id != a.state_id) {
            op_it +=
                state_info_it->num_outer_acts + state_info_it->num_inner_acts;
            ++state_info_it;
        }

        while (*op_it != a.action_id) {
            ++op_it;
        }

        return op_it - info->aops.begin();
    }

    QAction get_action(StateID sid, ActionID aid) override
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            return QAction(sid, aid);
        }

        auto state_info_it = info->state_infos.begin();
        unsigned sum =
            state_info_it->num_outer_acts + state_info_it->num_inner_acts;

        while (sum <= aid) {
            ++state_info_it;
            sum +=
                state_info_it->num_outer_acts + state_info_it->num_inner_acts;
        }

        return QAction(state_info_it->state_id, info->aops[aid]);
    }

    void generate_applicable_actions(StateID sid, std::vector<QAction>& aops)
        override
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig;
            state_space_->generate_applicable_actions(sid, orig);

            aops.reserve(orig.size());

            for (const Action& a : orig) {
                aops.emplace_back(sid, state_space_->get_action_id(sid, a));
            }
        } else {
            aops.reserve(info->total_num_outer_acts);

            auto aid = info->aops.begin();

            for (const auto& sinfo : info->state_infos) {
                const auto outers_end = aid + sinfo.num_outer_acts;
                for (; aid != outers_end; ++aid) {
                    aops.emplace_back(sinfo.state_id, *aid);
                }
                aid += sinfo.num_inner_acts; // Skip inner actions
            }

            assert(aops.size() == info->total_num_outer_acts);
        }
    }

    void generate_action_transitions(
        StateID,
        QAction a,
        Distribution<StateID>& result) override
    {
        const auto act = state_space_->get_action(a.state_id, a.action_id);

        Distribution<StateID> orig;
        state_space_->generate_action_transitions(a.state_id, act, orig);

        for (const auto& [state_id, probability] : orig) {
            result.add_probability(
                get_masked_state_id(state_id) & MASK,
                probability);
        }
    }

    void generate_all_transitions(
        StateID sid,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors) override
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig_a;
            state_space_->generate_applicable_actions(sid, orig_a);

            aops.reserve(orig_a.size());
            successors.reserve(orig_a.size());

            for (unsigned i = 0; i < orig_a.size(); ++i) {
                ActionID aid = state_space_->get_action_id(sid, orig_a[i]);
                const QAction& a = aops.emplace_back(sid, aid);
                generate_action_transitions(sid, a, successors.emplace_back());
            }
        } else {
            aops.reserve(info->total_num_outer_acts);
            successors.reserve(info->total_num_outer_acts);

            auto aop = info->aops.begin();

            for (const auto& info : info->state_infos) {
                const auto outers_end = aop + info.num_outer_acts;
                for (; aop != outers_end; ++aop) {
                    const QAction& a = aops.emplace_back(info.state_id, *aop);
                    generate_action_transitions(
                        sid,
                        a,
                        successors.emplace_back());
                }
                aop += info.num_inner_acts; // Skip inner actions
            }

            assert(aops.size() == info->total_num_outer_acts);
            assert(successors.size() == info->total_num_outer_acts);
        }
    }

    engine_interfaces::StateSpace<State, Action>* get_parent_state_space()
    {
        return state_space_;
    }

    const_iterator begin() const { return const_iterator(this, 0); }

    const_iterator end() const
    {
        return const_iterator(this, quotient_ids_.size());
    }

    unsigned quotient_size(StateID state_id) const
    {
        const QuotientInformation* info = get_quotient_info(state_id);
        return info ? info->state_infos.size() : 1;
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

    void get_pruned_ops(StateID sid, std::vector<QAction>& aops) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            // No quotient, no pruned actions.
            return;
        }

        aops.reserve(info->aops.size() - info->total_num_outer_acts);

        auto aid = info->aops.begin();

        for (const auto& sinfo : info->state_infos) {
            aid += sinfo.num_outer_acts; // Start with inner actions
            const auto inners_end = aid + sinfo.num_inner_acts;
            for (; aid != inners_end; ++aid) {
                aops.emplace_back(sinfo.state_id, *aid);
            }
        }

        assert(aops.size() == info->aops.size() - info->total_num_outer_acts);
    }

    StateID translate_state_id(StateID sid) const
    {
        return StateID(get_masked_state_id(sid) & MASK);
    }

    ActionID get_original_action_id(StateID sid, ActionID a) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        return info ? info->aops[a] : a;
    }

    Action get_original_action(StateID, const QAction& a) const
    {
        return state_space_->get_action(a.state_id, a.action_id);
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
            utils::infinite_iterator<std::vector<QAction>>());
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
        if (qinfo.state_infos.empty()) {
            // Add this state to the quotient
            auto& b = qinfo.state_infos.emplace_back(rid);
            set_masked_state_id(rid, rid);

            // Generate the applicable actions
            std::vector<Action> gen_aops;
            state_space_->generate_applicable_actions(rid, gen_aops);

            const size_t prev_size = qinfo.aops.size();

            // Add the action ids to the new quotient
            for (const Action& a : gen_aops) {
                qinfo.aops.push_back(state_space_->get_action_id(rid, a));
            }

            // Partition action ids
            auto inner_it = partition_actions(
                qinfo.aops.begin() + prev_size,
                qinfo.aops.end(),
                ignore_actions[ridx] |
                    std::views::transform(&QAction::action_id));

            b.num_outer_acts =
                std::distance(qinfo.aops.begin() + prev_size, inner_it);
            b.num_inner_acts = std::distance(inner_it, qinfo.aops.end());

            qinfo.total_num_outer_acts += b.num_outer_acts;
        } else {
            // Filter actions
            qinfo.filter_actions(*state_space_, ignore_actions[ridx]);
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
                q.filter_actions(*state_space_, *ignore_actions);

                // Insert all states belonging to it to the new quotient
                for (const auto& p : q.state_infos) {
                    qinfo.state_infos.push_back(p);
                    set_masked_state_id(p.state_id, rid);
                }

                // Move the actions to the new quotient
                std::ranges::move(q.aops, std::back_inserter(qinfo.aops));
                qinfo.total_num_outer_acts += q.total_num_outer_acts;

                // Erase the old quotient
                quotients_.erase(qit);
            } else {
                // Add this state to the quotient
                auto& b = qinfo.state_infos.emplace_back(state_id);
                set_masked_state_id(state_id, rid);

                // Generate the applicable actions
                std::vector<Action> gen_aops;
                state_space_->generate_applicable_actions(state_id, gen_aops);

                const size_t prev_size = qinfo.aops.size();

                // Add the action ids to the new quotient
                for (const Action& a : gen_aops) {
                    qinfo.aops.push_back(
                        state_space_->get_action_id(state_id, a));
                }

                // Partition actions
                auto inner_it = partition_actions(
                    qinfo.aops.begin() + prev_size,
                    qinfo.aops.end(),
                    *ignore_actions |
                        std::views::transform(&QAction::action_id));

                b.num_outer_acts =
                    std::distance(qinfo.aops.begin() + prev_size, inner_it);
                b.num_inner_acts = std::distance(inner_it, qinfo.aops.end());

                qinfo.total_num_outer_acts += b.num_outer_acts;
            }
        }
    }

    template <typename StateIDIterator, typename IgnoreActionsIterator>
    void build_new_quotient(
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
        assert(qinfo.state_infos.empty());

        // Add this state to the quotient
        auto& b = qinfo.state_infos.emplace_back(rid);
        set_masked_state_id(rid, rid);

        // Generate the applicable actions
        std::vector<Action> gen_aops;
        state_space_->generate_applicable_actions(rid, gen_aops);

        // Partition actions
        auto inner_it = partition_actions(gen_aops, ignore_actions[ridx]);

        // Add the action ids to the new quotient
        for (const Action& a : gen_aops) {
            qinfo.aops.push_back(state_space_->get_action_id(rid, a));
        }

        b.num_outer_acts = std::distance(gen_aops.begin(), inner_it);
        b.num_inner_acts = std::distance(inner_it, gen_aops.end());

        qinfo.total_num_outer_acts += b.num_outer_acts;

        for (auto it = begin; it != end; ++it, ++ignore_actions) {
            const auto& state_id = *it;

            // Already handled.
            if (state_id == rid) {
                continue;
            }

            assert(!(get_masked_state_id(state_id) & FLAG));

            // Add this state to the quotient
            auto& b = qinfo.state_infos.emplace_back(state_id);
            set_masked_state_id(state_id, rid);

            // Generate the applicable actions
            std::vector<Action> gen_aops;
            state_space_->generate_applicable_actions(state_id, gen_aops);

            // Partition actions
            auto inner_it = partition_actions(gen_aops, *ignore_actions);

            // Add the action ids to the new quotient
            for (const Action& a : gen_aops) {
                qinfo.aops.push_back(state_space_->get_action_id(state_id, a));
            }

            b.num_outer_acts = std::distance(gen_aops.begin(), inner_it);
            b.num_inner_acts = std::distance(inner_it, gen_aops.end());

            qinfo.total_num_outer_acts += b.num_outer_acts;
        }
    }

private:
    auto partition_actions(
        std::vector<Action>& aops,
        const std::vector<Action>& filter) const
    {
        if (filter.empty()) {
            return aops.end();
        }

        return std::stable_partition(
            aops.begin(),
            aops.end(),
            [&filter](const Action& a) { return !utils::contains(filter, a); });
    }

    auto partition_actions(
        std::vector<ActionID>::iterator aops_begin,
        std::vector<ActionID>::iterator aops_end,
        const auto& filter) const
    {
        if (filter.empty()) {
            return aops_end;
        }

        return std::stable_partition(
            aops_begin,
            aops_end,
            [&filter](ActionID action_id) {
                return !utils::contains(filter, action_id);
            });
    }

    QuotientInformation* get_quotient_info(StateID state_id)
    {
        const StateID::size_type qid = get_masked_state_id(state_id);
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

/*
template <typename State>
class QuotientSystem<State, OperatorID>;
*/

} // namespace quotients

template <typename Action>
struct is_cheap_to_copy<quotients::QuotientAction<Action>> : std::true_type {};

} // namespace probfd

#endif // __QUOTIENT_SYSTEM_H__
