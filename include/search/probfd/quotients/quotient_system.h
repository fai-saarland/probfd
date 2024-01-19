#ifndef PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
#define PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H

#include "probfd/utils/language.h"

#include "probfd/mdp.h"

#include "downward/algorithms/segmented_vector.h"

#include <compare>
#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace probfd::quotients {

template <typename State, typename Action>
class QuotientSystem;

template <typename State, typename Action>
struct QuotientState;

template <typename Action>
struct QuotientAction {
    StateID state_id;
    Action action;

    friend auto
    operator<=>(const QuotientAction&, const QuotientAction&) = default;
};

template <typename Action>
class QuotientInformation {
    template <typename, typename>
    friend class QuotientSystem;

    template <typename, typename>
    friend struct QuotientState;

    struct StateInfo;

    std::vector<StateInfo> state_infos_;
    std::vector<Action> aops_; // First outer, then inner actions
    size_t total_num_outer_acts_ = 0;
    TerminationInfo termination_info_;

    [[nodiscard]]
    size_t num_members() const;

    auto member_ids();
    auto member_ids() const;

    void filter_actions(const std::vector<QuotientAction<Action>>& filter);
};

template <typename State, typename Action>
struct QuotientState {
    template <typename, typename>
    friend class QuotientSystem;

    using QuotientInformation = QuotientInformation<Action>;
    using MDP = MDP<State, Action>;

    MDP& mdp;
    std::variant<State, const QuotientInformation*> single_or_quotient;

    explicit QuotientState(MDP& mdp, State single);
    explicit QuotientState(MDP& mdp, const QuotientInformation* quotient);

public:
    template <std::invocable<param_type<State>> F>
    value_t member_maximum(F&& f) const
        requires(std::is_convertible_v<
                 std::invoke_result_t<F, param_type<State>>,
                 value_t>);

    void
    for_each_member_state(std::invocable<param_type<State>> auto&& f) const;

    [[nodiscard]]
    size_t num_members() const;

    void get_collapsed_actions(std::vector<QuotientAction<Action>>& aops) const;
};

template <typename State, typename Action>
class QuotientSystem
    : public MDP<QuotientState<State, Action>, QuotientAction<Action>> {
    friend struct const_iterator;

    using QuotientInformation = QuotientInformation<Action>;
    using QState = QuotientState<State, Action>;
    using QAction = QuotientAction<Action>;

    using MDP = MDP<State, Action>;

    std::unordered_map<StateID::size_type, QuotientInformation> quotients_;
    segmented_vector::SegmentedVector<StateID::size_type> quotient_ids_;
    MDP& mdp_;

    // MASK: bitmask used to obtain the quotient state id, if it exists
    // FLAG: whether a quotient state id exists
    static constexpr StateID::size_type MASK = (StateID::size_type(-1) >> 1);
    static constexpr StateID::size_type FLAG = ~MASK;

public:
    class const_iterator : public add_postfix_inc_dec<const_iterator> {
        const QuotientSystem* qs_ = nullptr;
        StateID i_;

    public:
        using add_postfix_inc_dec<const_iterator>::operator++;

        using difference_type = std::ptrdiff_t;
        using value_type = StateID;

        const_iterator() = default;
        const_iterator(const QuotientSystem* qs, StateID x);

        const_iterator& operator++();
        StateID operator*() const;

        friend bool
        operator==(const const_iterator& left, const const_iterator& right);
    };

    static_assert(std::input_iterator<const_iterator>);

    explicit QuotientSystem(MDP& mdp);

    StateID get_state_id(param_type<QState> state) override;

    QState get_state(StateID state_id) override;

    void generate_applicable_actions(
        param_type<QState> state,
        std::vector<QAction>& aops) override;

    void generate_action_transitions(
        param_type<QState>,
        QAction a,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        param_type<QState> state,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    void generate_all_transitions(
        param_type<QState> state,
        std::vector<Transition<QAction>>& transitions) override;

    TerminationInfo get_termination_info(param_type<QState> s) override;

    value_t get_action_cost(QAction qa) override;

    MDP& get_parent_mdp();

    const_iterator begin() const;
    const_iterator end() const;

    QState translate_state(param_type<State> s) const;

    [[nodiscard]]
    StateID translate_state_id(StateID sid) const;

    template <typename Range>
    void build_quotient(Range& states);

    template <typename SubMDP>
    void
    build_quotient(SubMDP submdp, std::ranges::range_reference_t<SubMDP> entry);

    template <typename SubMDP>
    void build_new_quotient(
        SubMDP submdp,
        std::ranges::range_reference_t<SubMDP> entry);

private:
    auto partition_actions(
        std::ranges::input_range auto&& aops,
        const std::ranges::input_range auto& filter) const;

    QuotientInformation* get_quotient_info(StateID state_id);
    const QuotientInformation* get_quotient_info(StateID state_id) const;

    [[nodiscard]]
    StateID::size_type get_masked_state_id(StateID sid) const;

    void set_masked_state_id(StateID sid, const StateID::size_type& qsid);
};

} // namespace probfd::quotients

namespace probfd {

template <typename Action>
struct is_cheap_to_copy<quotients::QuotientAction<Action>> : std::true_type {};

} // namespace probfd

#define GUARD_INCLUDE_PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
#include "probfd/quotients/quotient_system_impl.h"
#undef GUARD_INCLUDE_PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H

#endif // PROBFD_QUOTIENTS_QUOTIENT_SYSTEM_H
