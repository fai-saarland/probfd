namespace probfd::policy_pickers {

template <typename State, typename Action>
ArbitraryTiebreaker<State, Action>::ArbitraryTiebreaker(bool stable_policy)
    : ArbitraryTiebreaker::StablePolicyPicker(stable_policy)
{
}

template <typename State, typename Action>
int ArbitraryTiebreaker<State, Action>::pick_index(
    MDP<State, Action>&,
    StateID,
    std::optional<Action>,
    const std::vector<Transition<Action>>&,
    algorithms::StateProperties&)
{
    return 0;
}

} // namespace probfd::policy_pickers
