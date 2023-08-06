#ifndef PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
#define PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H

#include "probfd/utils/iterators.h"

#include "probfd/distribution.h"
#include "probfd/mdp.h"

#include "downward/algorithms/segmented_vector.h"

#include "downward/utils/collections.h"

#include "downward/task_proxy.h"

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
    Action action;

    friend auto
    operator<=>(const QuotientAction&, const QuotientAction&) = default;
};

template <typename State, typename Action>
class DefaultQuotientSystem
    : public MDP<State, quotients::QuotientAction<Action>> {
    friend struct const_iterator;

    struct QuotientInformation {
        struct StateInfo {
            StateID state_id;
            size_t num_outer_acts = 0;
            size_t num_inner_acts = 0;
        };

        std::vector<StateInfo> state_infos;
        std::vector<Action> aops; // First outer, then inner actions
        size_t total_num_outer_acts = 0;

        auto begin() { return state_infos.begin(); }
        auto end() { return state_infos.end(); }

        auto begin() const { return state_infos.begin(); }
        auto end() const { return state_infos.end(); }

        auto state_begin()
        {
            return iterators::make_transform_iterator(
                begin(),
                &StateInfo::state_id);
        }

        auto state_end()
        {
            return iterators::make_transform_iterator(
                end(),
                &StateInfo::state_id);
        }

        auto state_begin() const
        {
            return iterators::make_transform_iterator(
                begin(),
                &StateInfo::state_id);
        }

        auto state_end() const
        {
            return iterators::make_transform_iterator(
                end(),
                &StateInfo::state_id);
        }

        void filter_actions(const std::vector<QuotientAction<Action>>& filter)
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
                    [&info, &filter](Action a) {
                        return !utils::contains(
                            filter,
                            QuotientAction<Action>(info.state_id, a));
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
    MDP<State, Action>* mdp_;

    // MASK: bitmask used to obtain the quotient state id, if it exists
    // FLAG: whether a quotient state id exists
    static constexpr StateID::size_type MASK = (StateID::size_type(-1) >> 1);
    static constexpr StateID::size_type FLAG = ~MASK;

public:
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator = iterators::variant_iterator<
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

    explicit DefaultQuotientSystem(MDP<State, Action>* mdp)
        : mdp_(mdp)
    {
    }

    StateID get_state_id(param_type<State> s) override
    {
        return mdp_->get_state_id(s);
    }

    State get_state(StateID sid) override { return mdp_->get_state(sid); }

    void generate_applicable_actions(StateID sid, std::vector<QAction>& aops)
        override
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (!info) {
            std::vector<Action> orig;
            mdp_->generate_applicable_actions(sid, orig);

            aops.reserve(orig.size());

            for (const Action& a : orig) {
                aops.emplace_back(sid, a);
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
        Distribution<StateID> orig;
        mdp_->generate_action_transitions(a.state_id, a.action, orig);

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
            mdp_->generate_applicable_actions(sid, orig_a);

            aops.reserve(orig_a.size());
            successors.reserve(orig_a.size());

            for (unsigned i = 0; i < orig_a.size(); ++i) {
                const QAction& a = aops.emplace_back(sid, orig_a[i]);
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

    void generate_all_transitions(
        StateID id,
        std::vector<Transition<QAction>>& transitions) override
    {
        const QuotientInformation* info = get_quotient_info(id);
        if (!info) {
            std::vector<Action> orig_a;
            mdp_->generate_applicable_actions(id, orig_a);

            transitions.reserve(orig_a.size());

            for (Action a : orig_a) {
                QAction qa(id, a);
                Transition<QAction>& t = transitions.emplace_back(qa);
                generate_action_transitions(id, qa, t.successor_dist);
            }
        } else {
            transitions.reserve(info->total_num_outer_acts);

            auto aop = info->aops.begin();

            for (const auto& info : info->state_infos) {
                const auto outers_end = aop + info.num_outer_acts;
                for (; aop != outers_end; ++aop) {
                    QAction qa(info.state_id, *aop);
                    Transition<QAction>& t = transitions.emplace_back(qa);
                    generate_action_transitions(id, qa, t.successor_dist);
                }
                aop += info.num_inner_acts; // Skip inner actions
            }

            assert(transitions.size() == info->total_num_outer_acts);
        }
    }

    TerminationInfo get_termination_info(param_type<State> s) override
    {
        return mdp_->get_termination_info(s);
    }

    value_t get_action_cost(QuotientAction<Action> qa) override
    {
        return mdp_->get_action_cost(qa.action);
    }

    MDP<State, Action>* get_parent_mdp() { return mdp_; }

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

    template <typename Range>
    void build_quotient(Range& states)
    {
        std::vector<std::vector<QAction>> dummy(states.size());
        auto range = std::views::zip(states, dummy);
        this->build_quotient(range, *range.begin());
    }

    template <typename SubMDP>
    void
    build_quotient(SubMDP submdp, std::ranges::range_reference_t<SubMDP> entry)
    {
        const StateID rid = get<0>(entry);
        const auto& raops = get<1>(entry);

        // Get or create quotient
        QuotientInformation& qinfo = quotients_[rid];

        // We handle the representative state first so that it
        // appears first in the data structure.
        if (qinfo.state_infos.empty()) {
            // Add this state to the quotient
            auto& b = qinfo.state_infos.emplace_back(rid);
            set_masked_state_id(rid, rid);

            const size_t prev_size = qinfo.aops.size();

            // Generate the applicable actions and add them to the new
            // quotient
            mdp_->generate_applicable_actions(rid, qinfo.aops);

            // Partition new actions
            auto new_aops = std::ranges::subrange(
                qinfo.aops.begin() + prev_size,
                qinfo.aops.end());

            {
                auto [pivot, last] = partition_actions(
                    new_aops,
                    raops | std::views::transform(&QAction::action));

                b.num_outer_acts = std::distance(new_aops.begin(), pivot);
                b.num_inner_acts = std::distance(pivot, last);
            }

            qinfo.total_num_outer_acts += b.num_outer_acts;
        } else {
            // Filter actions
            qinfo.filter_actions(raops);
        }

        for (const auto& entry : submdp) {
            const StateID state_id = get<0>(entry);
            const auto& aops = get<1>(entry);

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
                q.filter_actions(aops);

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

                const size_t prev_size = qinfo.aops.size();

                // Generate the applicable actions and add them to the new
                // quotient
                mdp_->generate_applicable_actions(state_id, qinfo.aops);

                // Partition new actions
                auto new_aops = std::ranges::subrange(
                    qinfo.aops.begin() + prev_size,
                    qinfo.aops.end());

                auto [pivot, last] = partition_actions(
                    new_aops,
                    aops | std::views::transform(&QAction::action));

                b.num_outer_acts = std::distance(new_aops.begin(), pivot);
                b.num_inner_acts = std::distance(pivot, last);

                qinfo.total_num_outer_acts += b.num_outer_acts;
            }
        }
    }

    template <typename SubMDP>
    void build_new_quotient(
        SubMDP submdp,
        std::ranges::range_reference_t<SubMDP> entry)
    {
        const StateID rid = get<0>(entry);
        const auto& raops = get<1>(entry);

        // Get or create quotient
        QuotientInformation& qinfo = quotients_[rid];

        // We handle the representative state first so that it
        // appears first in the data structure.
        assert(qinfo.state_infos.empty());

        // Add this state to the quotient
        auto& b = qinfo.state_infos.emplace_back(rid);
        set_masked_state_id(rid, rid);

        // Generate the applicable actions
        mdp_->generate_applicable_actions(rid, qinfo.aops);

        // Partition actions
        {
            auto [pivot, last] = partition_actions(qinfo.aops, raops);

            b.num_outer_acts = std::distance(qinfo.aops.begin(), pivot);
            b.num_inner_acts = std::distance(pivot, last);

            qinfo.total_num_outer_acts += b.num_outer_acts;
        }

        for (const auto& entry : submdp) {
            const StateID state_id = get<0>(entry);
            const auto& aops = get<1>(entry);

            // Already handled.
            if (state_id == rid) {
                continue;
            }

            assert(!(get_masked_state_id(state_id) & FLAG));

            // Add this state to the quotient
            auto& b = qinfo.state_infos.emplace_back(state_id);
            set_masked_state_id(state_id, rid);

            // Generate the applicable actions
            mdp_->generate_applicable_actions(state_id, qinfo.aops);

            // Partition actions
            auto [pivot, last] = partition_actions(qinfo.aops, aops);

            b.num_outer_acts = std::distance(qinfo.aops.begin(), pivot);
            b.num_inner_acts = std::distance(pivot, last);

            qinfo.total_num_outer_acts += b.num_outer_acts;
        }
    }

private:
    auto partition_actions(
        std::ranges::input_range auto&& aops,
        const std::ranges::input_range auto& filter) const
    {
        if (filter.empty()) {
            return std::ranges::subrange(aops.begin(), aops.end());
        }

        return std::ranges::stable_partition(
            aops,
            [&filter](const Action& action) {
                return std::ranges::find(filter, action) == filter.end();
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
