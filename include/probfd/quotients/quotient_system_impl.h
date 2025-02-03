#ifndef GUARD_INCLUDE_PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
#error "This file should only be included from quotient_system.h"
#endif

#include "probfd/aliases.h"
#include "probfd/distribution.h"
#include "probfd/transition.h"

#include "downward/utils/collections.h"

namespace probfd::quotients {

template <typename Action>
struct QuotientInformation<Action>::StateInfo {
    StateID state_id;
    size_t num_outer_acts = 0;
    size_t num_inner_acts = 0;
};

template <typename Action>
size_t QuotientInformation<Action>::num_members() const
{
    return state_infos_.size();
}

template <typename Action>
auto QuotientInformation<Action>::member_ids()
{
    return std::views::transform(state_infos_, &StateInfo::state_id);
}

template <typename Action>
auto QuotientInformation<Action>::member_ids() const
{
    return std::views::transform(state_infos_, &StateInfo::state_id);
}

template <typename Action>
void QuotientInformation<Action>::filter_actions(
    const std::ranges::input_range auto& filter)
{
    if (std::ranges::empty(filter)) {
        return;
    }

    total_num_outer_acts_ = 0;

    auto act_it = aops_.begin();

    for (auto& info : state_infos_) {
        auto outer_end = std::stable_partition(
            act_it,
            act_it + info.num_outer_acts,
            [&info, &filter](Action a) {
                return !std::ranges::contains(
                    filter,
                    QuotientAction<Action>(info.state_id, a));
            });

        const size_t num_total_acts = info.num_outer_acts + info.num_inner_acts;

        info.num_outer_acts = std::distance(act_it, outer_end);
        info.num_inner_acts = num_total_acts - info.num_outer_acts;

        total_num_outer_acts_ += info.num_outer_acts;

        act_it += num_total_acts;
    }

    assert(act_it == aops_.end());
}

template <typename State, typename Action>
QuotientState<State, Action>::QuotientState(MDPType& mdp, State single)
    : mdp(mdp)
    , single_or_quotient(std::move(single))
{
}

template <typename State, typename Action>
QuotientState<State, Action>::QuotientState(
    MDPType& mdp,
    const QuotientInformationType* quotient)
    : mdp(mdp)
    , single_or_quotient(quotient)
{
}

template <typename State, typename Action>
template <std::invocable<ParamType<State>> F>
value_t QuotientState<State, Action>::member_maximum(F&& f) const
    requires(std::is_convertible_v<
             std::invoke_result_t<F, ParamType<State>>,
             value_t>)
{
    using namespace std::views;

    return std::visit(
        overloaded{
            [&](const QuotientInformationType* quotient) {
                value_t res = -INFINITE_VALUE;
                for (ParamType<State> state :
                     quotient->member_ids() | transform(std::bind_front(
                                                  &MDPType::get_state,
                                                  std::ref(mdp)))) {
                    res = std::max(res, f(state));
                }
                return res;
            },
            [&](ParamType<State> single) { return f(single); }},
        single_or_quotient);
}

template <typename State, typename Action>
void QuotientState<State, Action>::for_each_member_state(
    std::invocable<ParamType<State>> auto&& f) const
{
    std::visit(
        overloaded{
            [&](const QuotientInformationType* quotient) {
                std::ranges::for_each(
                    quotient->member_ids() |
                        std::views::transform(std::bind_front(
                            &MDPType::get_state,
                            std::ref(mdp))),
                    f);
            },
            [&](ParamType<State> single) { f(single); }},
        single_or_quotient);
}

template <typename State, typename Action>
size_t QuotientState<State, Action>::num_members() const
{
    return std::visit(
        overloaded{
            [](const QuotientInformationType* quotient) {
                return quotient->num_members();
            },
            [](ParamType<State>) -> size_t { return 1; }},
        single_or_quotient);
}

template <typename State, typename Action>
void QuotientState<State, Action>::get_collapsed_actions(
    std::vector<QuotientAction<Action>>& aops) const
{
    std::visit(
        overloaded{
            [&](const QuotientInformationType* info) {
                aops.reserve(info->aops_.size() - info->total_num_outer_acts_);

                auto aid = info->aops_.begin();

                for (const auto& sinfo : info->state_infos_) {
                    aid += sinfo.num_outer_acts; // Start with inner actions
                    const auto inners_end = aid + sinfo.num_inner_acts;
                    for (; aid != inners_end; ++aid) {
                        aops.emplace_back(sinfo.state_id, *aid);
                    }
                }

                assert(
                    aops.size() ==
                    info->aops_.size() - info->total_num_outer_acts_);
            },
            [](ParamType<State>) { return; }},
        single_or_quotient);
}

template <typename State, typename Action>
quotient_id_iterator<State, Action>::quotient_id_iterator(
    const QuotientSystem<State, Action>* qs,
    StateID x)
    : qs_(qs)
    , i_(x)

{
}

template <typename State, typename Action>
auto quotient_id_iterator<State, Action>::operator++() -> quotient_id_iterator&
{
    while (++i_.id < qs_->quotient_ids_.size()) {
        const StateID ref = qs_->quotient_ids_[i_];
        if (i_ == (ref & QuotientSystem<State, Action>::MASK)) {
            break;
        }
    }

    return *this;
}

template <typename State, typename Action>
bool operator==(
    const typename QuotientSystem<State, Action>::quotient_id_iterator& left,
    const typename QuotientSystem<State, Action>::quotient_id_iterator& right)
{
    return left.i_ == right.i_;
}

template <typename State, typename Action>
StateID quotient_id_iterator<State, Action>::operator*() const
{
    return i_;
}

template <typename State, typename Action>
QuotientSystem<State, Action>::QuotientSystem(MDPType& mdp)
    : mdp_(mdp)
{
}

template <typename State, typename Action>
StateID QuotientSystem<State, Action>::get_state_id(ParamType<QState> state)
{
    return std::visit(
        overloaded{
            [&](const QuotientInformationType* info) {
                return info->state_infos_.front().state_id;
            },
            [&](ParamType<State> s) { return mdp_.get_state_id(s); }},
        state.single_or_quotient);
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::get_state(StateID state_id) -> QState
{
    const QuotientInformationType* info = get_quotient_info(state_id);

    if (info) {
        return QState(mdp_, info);
    }

    return QState(mdp_, mdp_.get_state(state_id));
}

template <typename State, typename Action>
void QuotientSystem<State, Action>::generate_applicable_actions(
    ParamType<QState> state,
    std::vector<QAction>& aops)
{
    std::visit(
        overloaded{
            [&](const QuotientInformationType* info) {
                aops.reserve(info->total_num_outer_acts_);

                auto aid = info->aops_.begin();

                for (const auto& sinfo : info->state_infos_) {
                    const auto outers_end = aid + sinfo.num_outer_acts;
                    for (; aid != outers_end; ++aid) {
                        aops.emplace_back(sinfo.state_id, *aid);
                    }
                    aid += sinfo.num_inner_acts; // Skip inner actions
                }

                assert(aops.size() == info->total_num_outer_acts_);
            },
            [&](ParamType<State> state) {
                std::vector<Action> orig;
                mdp_.generate_applicable_actions(state, orig);

                const StateID state_id = mdp_.get_state_id(state);
                aops.reserve(orig.size());

                for (const Action& a : orig) {
                    aops.emplace_back(state_id, a);
                }
            }},
        state.single_or_quotient);
}

template <typename State, typename Action>
void QuotientSystem<State, Action>::generate_action_transitions(
    ParamType<QState>,
    QAction a,
    Distribution<StateID>& result)
{
    Distribution<StateID> orig;
    const State state = this->mdp_.get_state(a.state_id);
    mdp_.generate_action_transitions(state, a.action, orig);

    for (const auto& [state_id, probability] : orig) {
        result.add_probability(
            get_masked_state_id(state_id) & MASK,
            probability);
    }
}

template <typename State, typename Action>
void QuotientSystem<State, Action>::generate_all_transitions(
    ParamType<QState> state,
    std::vector<QAction>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    std::visit(
        overloaded{
            [&](const QuotientInformationType* info) {
                aops.reserve(info->total_num_outer_acts_);
                successors.reserve(info->total_num_outer_acts_);

                auto aop = info->aops_.begin();

                for (const auto& info : info->state_infos_) {
                    const auto outers_end = aop + info.num_outer_acts;
                    for (; aop != outers_end; ++aop) {
                        const QAction& a =
                            aops.emplace_back(info.state_id, *aop);
                        generate_action_transitions(
                            state,
                            a,
                            successors.emplace_back());
                    }
                    aop += info.num_inner_acts; // Skip inner actions
                }

                assert(aops.size() == info->total_num_outer_acts_);
                assert(successors.size() == info->total_num_outer_acts_);
            },
            [&](ParamType<State> state) {
                std::vector<Action> orig_a;
                mdp_.generate_applicable_actions(state, orig_a);

                const StateID state_id = mdp_.get_state_id(state);
                aops.reserve(orig_a.size());
                successors.reserve(orig_a.size());

                for (Action oa : orig_a) {
                    aops.emplace_back(state_id, oa);
                    auto& dist = successors.emplace_back();

                    Distribution<StateID> orig;
                    mdp_.generate_action_transitions(state, oa, orig);

                    for (const auto& [state_id, probability] : orig) {
                        dist.add_probability(
                            get_masked_state_id(state_id) & MASK,
                            probability);
                    }
                }
            }},
        state.single_or_quotient);
}

template <typename State, typename Action>
void QuotientSystem<State, Action>::generate_all_transitions(
    ParamType<QState> state,
    std::vector<Transition<QAction>>& transitions)
{
    std::visit(
        overloaded{
            [&](const QuotientInformationType* info) {
                transitions.reserve(info->total_num_outer_acts_);

                auto aop = info->aops_.begin();

                for (const auto& info : info->state_infos_) {
                    const auto outers_end = aop + info.num_outer_acts;
                    for (; aop != outers_end; ++aop) {
                        QAction qa(info.state_id, *aop);
                        Transition<QAction>& t = transitions.emplace_back(qa);
                        generate_action_transitions(
                            state,
                            qa,
                            t.successor_dist);
                    }
                    aop += info.num_inner_acts; // Skip inner actions
                }

                assert(transitions.size() == info->total_num_outer_acts_);
            },
            [&](ParamType<State> state) {
                std::vector<Action> orig_a;
                mdp_.generate_applicable_actions(state, orig_a);

                const StateID state_id = mdp_.get_state_id(state);
                transitions.reserve(orig_a.size());

                for (Action a : orig_a) {
                    QAction qa(state_id, a);
                    Transition<QAction>& t = transitions.emplace_back(qa);

                    Distribution<StateID> orig;
                    mdp_.generate_action_transitions(state, a, orig);

                    for (const auto& [state_id, probability] : orig) {
                        t.successor_dist.add_probability(
                            get_masked_state_id(state_id) & MASK,
                            probability);
                    }
                }
            }},
        state.single_or_quotient);
}

template <typename State, typename Action>
TerminationInfo
QuotientSystem<State, Action>::get_termination_info(ParamType<QState> s)
{
    return std::visit(
        overloaded{
            [&](const QuotientInformationType* info) {
                return info->termination_info_;
            },
            [&](ParamType<State> state) {
                return mdp_.get_termination_info(state);
            }},
        s.single_or_quotient);
}

template <typename State, typename Action>
value_t QuotientSystem<State, Action>::get_action_cost(QAction qa)
{
    return mdp_.get_action_cost(qa.action);
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::get_parent_mdp() -> MDPType&
{
    return mdp_;
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::begin() const -> const_iterator
{
    return quotient_id_iterator(this, 0);
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::end() const -> const_iterator
{
    return quotient_id_iterator(this, quotient_ids_.size());
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::translate_state(ParamType<State> s) const
    -> QState
{
    StateID id = mdp_.get_state_id(s);
    const auto* info = get_quotient_info(get_masked_state_id(id));

    if (info) {
        return QState(mdp_, info);
    }

    return QState(mdp_, s);
}

template <typename State, typename Action>
StateID QuotientSystem<State, Action>::translate_state_id(StateID sid) const
{
    return StateID(get_masked_state_id(sid) & MASK);
}

template <typename State, typename Action>
template <typename Range>
void QuotientSystem<State, Action>::build_quotient(Range& states)
{
    auto range =
        std::views::zip(states, std::views::repeat(std::vector<QAction>()));
    this->build_quotient(range, *range.begin());
}

template <typename State, typename Action>
template <typename SubMDP>
void QuotientSystem<State, Action>::build_quotient(
    SubMDP submdp,
    std::ranges::range_reference_t<SubMDP> entry)
{
    using namespace std::views;

    const StateID rid = get<0>(entry);
    const auto& raops = get<1>(entry);

    value_t min_termination = INFINITE_VALUE;
    bool is_goal = false;

    // Get or create quotient
    QuotientInformationType& qinfo = quotients_[rid];

    // We handle the representative state first so that it
    // appears first in the data structure.
    if (qinfo.state_infos_.empty()) {
        // Add this state to the quotient
        auto& b = qinfo.state_infos_.emplace_back(rid);
        set_masked_state_id(rid, rid);

        const State repr = mdp_.get_state(rid);

        // Merge goal state status and termination cost
        const auto repr_term = mdp_.get_termination_info(repr);
        min_termination = std::min(min_termination, repr_term.get_cost());
        is_goal = is_goal || repr_term.is_goal_state();

        // Generate the applicable actions and add them to the new
        // quotient
        const size_t prev_size = qinfo.aops_.size();
        mdp_.generate_applicable_actions(repr, qinfo.aops_);

        // Partition new actions
        auto new_aops = qinfo.aops_ | drop(prev_size);

        {
            auto [pivot, last] = partition_actions(
                new_aops,
                raops | transform(&QAction::action));

            b.num_outer_acts = std::distance(new_aops.begin(), pivot);
            b.num_inner_acts = std::distance(pivot, last);
        }

        qinfo.total_num_outer_acts_ += b.num_outer_acts;
    } else {
        // Filter actions
        qinfo.filter_actions(raops);

        // Merge goal state status and termination cost
        const auto repr_term = qinfo.termination_info_;
        min_termination = std::min(min_termination, repr_term.get_cost());
        is_goal = is_goal || repr_term.is_goal_state();
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
            QuotientInformationType& q = qit->second;

            // Filter actions
            q.filter_actions(aops);

            // Merge goal state status and termination cost
            const auto mem_term = q.termination_info_;
            min_termination = std::min(min_termination, mem_term.get_cost());
            is_goal = is_goal || mem_term.is_goal_state();

            // Insert all states belonging to it to the new quotient
            for (const auto& p : q.state_infos_) {
                qinfo.state_infos_.push_back(p);
                set_masked_state_id(p.state_id, rid);
            }

            // Move the actions to the new quotient
            std::ranges::move(q.aops_, std::back_inserter(qinfo.aops_));
            qinfo.total_num_outer_acts_ += q.total_num_outer_acts_;

            // Erase the old quotient
            quotients_.erase(qit);
        } else {
            // Add this state to the quotient
            auto& b = qinfo.state_infos_.emplace_back(state_id);
            set_masked_state_id(state_id, rid);

            const State mem = mdp_.get_state(state_id);

            // Merge goal state status and termination cost
            const auto mem_term = mdp_.get_termination_info(mem);
            min_termination = std::min(min_termination, mem_term.get_cost());
            is_goal = is_goal || mem_term.is_goal_state();

            // Generate the applicable actions and add them to the new
            // quotient
            const size_t prev_size = qinfo.aops_.size();
            mdp_.generate_applicable_actions(mem, qinfo.aops_);

            // Partition new actions
            auto new_aops = qinfo.aops_ | drop(prev_size);

            auto [pivot, last] = partition_actions(
                new_aops,
                aops | std::views::transform(&QAction::action));

            b.num_outer_acts = std::distance(new_aops.begin(), pivot);
            b.num_inner_acts = std::distance(pivot, last);

            qinfo.total_num_outer_acts_ += b.num_outer_acts;
        }
    }

    qinfo.termination_info_ = TerminationInfo(is_goal, min_termination);
}

template <typename State, typename Action>
template <typename SubMDP>
void QuotientSystem<State, Action>::build_new_quotient(
    SubMDP submdp,
    std::ranges::range_reference_t<SubMDP> entry)
{
    const StateID rid = get<0>(entry);
    const auto& raops = get<1>(entry);

    // Get or create quotient
    QuotientInformationType& qinfo = quotients_[rid];

    // We handle the representative state first so that it
    // appears first in the data structure.
    assert(qinfo.state_infos_.empty());

    // Merged goal state status and termination cost
    value_t min_termination;
    bool is_goal;

    {
        // Add this state to the quotient
        auto& b = qinfo.state_infos_.emplace_back(rid);
        set_masked_state_id(rid, rid);

        const State repr = mdp_.get_state(rid);

        // Merge goal state status and termination cost
        const auto repr_term = mdp_.get_termination_info(repr);
        min_termination = repr_term.get_cost();
        is_goal = repr_term.is_goal_state();

        // Generate the applicable actions
        mdp_.generate_applicable_actions(repr, qinfo.aops_);

        // Partition actions
        auto [pivot, last] = partition_actions(qinfo.aops_, raops);

        b.num_outer_acts = std::distance(qinfo.aops_.begin(), pivot);
        b.num_inner_acts = std::distance(pivot, last);

        qinfo.total_num_outer_acts_ += b.num_outer_acts;
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
        auto& b = qinfo.state_infos_.emplace_back(state_id);
        set_masked_state_id(state_id, rid);

        const State mem = mdp_.get_state(state_id);

        // Merge goal state status and termination cost
        const auto mem_term = mdp_.get_termination_info(mem);
        min_termination = std::min(min_termination, mem_term.get_cost());
        is_goal = is_goal || mem_term.is_goal_state();

        // Generate the applicable actions
        mdp_.generate_applicable_actions(mem, qinfo.aops_);

        // Partition actions
        auto [pivot, last] = partition_actions(qinfo.aops_, aops);

        b.num_outer_acts = std::distance(qinfo.aops_.begin(), pivot);
        b.num_inner_acts = std::distance(pivot, last);

        qinfo.total_num_outer_acts_ += b.num_outer_acts;
    }

    qinfo.termination_info_ = TerminationInfo(is_goal, min_termination);
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::partition_actions(
    std::ranges::input_range auto&& aops,
    const std::ranges::input_range auto& filter) const
{
    if (filter.empty()) {
        return std::ranges::subrange(aops.begin(), aops.end());
    }

    return std::ranges::stable_partition(aops, [&filter](const Action& action) {
        return std::ranges::find(filter, action) == filter.end();
    });
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::get_quotient_info(StateID state_id)
    -> QuotientInformationType*
{
    const StateID::size_type qid = get_masked_state_id(state_id);
    return qid & FLAG ? &quotients_.find(qid & MASK)->second : nullptr;
}

template <typename State, typename Action>
auto QuotientSystem<State, Action>::get_quotient_info(StateID state_id) const
    -> const QuotientInformationType*
{
    const StateID::size_type qid = get_masked_state_id(state_id);
    return qid & FLAG ? &quotients_.find(qid & MASK)->second : nullptr;
}

template <typename State, typename Action>
StateID::size_type
QuotientSystem<State, Action>::get_masked_state_id(StateID sid) const
{
    return sid < quotient_ids_.size() ? quotient_ids_[sid] : sid.id;
}

template <typename State, typename Action>
void QuotientSystem<State, Action>::set_masked_state_id(
    StateID sid,
    const StateID::size_type& qsid)
{
    if (sid >= quotient_ids_.size()) {
        for (auto idx = quotient_ids_.size(); idx <= sid; ++idx) {
            quotient_ids_.push_back(idx);
        }
    }

    quotient_ids_[sid] = qsid | FLAG;
}

} // namespace probfd::quotients
