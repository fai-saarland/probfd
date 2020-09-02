#include "policy_chooser.h"

#include "../plugin.h"

#include <cassert>

namespace probabilistic {
namespace algorithms {

unsigned
PolicyChooser<GlobalState, const ProbabilisticOperator*>::operator()(
    const bool,
    const ProbabilisticOperator* const* prev,
    const GlobalState& state,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        greedy_optimal)
{
    assert(prev != nullptr);
    assert(!greedy_optimal.empty());
    return this->choose(state, *prev, greedy_optimal);
}

unsigned
PolicyChooser<GlobalState, const ProbabilisticOperator*>::choose(
    const GlobalState& state,
    const ProbabilisticOperator* prev,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        greedy_optimal)
{
    for (int i = greedy_optimal.size() - 1; i >= 0; --i) {
        if (greedy_optimal[i].first == prev) {
            return i;
        }
    }
    return this->choose(state, greedy_optimal);
}

unsigned
PolicyChooser<GlobalState, const ProbabilisticOperator*>::choose(
    const GlobalState&,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&)
{
    return 0;
}

unsigned
PolicyChooser<QuotientState, QuotientAction>::operator()(
    const bool,
    const QuotientAction* prev,
    const QuotientState& state,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        greedy_optimal)
{
    assert(prev != nullptr);
    assert(!greedy_optimal.empty());
    return this->choose(state, *prev, greedy_optimal);
}

unsigned
PolicyChooser<QuotientState, QuotientAction>::choose(
    const QuotientState& state,
    const QuotientAction& prev,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        greedy_optimal)
{
    for (int i = greedy_optimal.size() - 1; i >= 0; --i) {
        if (greedy_optimal[i].first == prev) {
            return i;
        }
    }
    return this->choose(state, greedy_optimal);
}

unsigned
PolicyChooser<QuotientState, QuotientAction>::choose(
    const QuotientState&,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&)
{
    return 0;
}

} // namespace algorithms

void
PolicyChooser::connect(algorithms::HeuristicSearchStatusInterface<
                       GlobalState,
                       const ProbabilisticOperator*>* i)
{
    hs_interface_ = i;
}

void
PolicyChooser::connect(
    algorithms::HeuristicSearchStatusInterface<QuotientState, QuotientAction>*
        i)
{
    qhs_interface_ = i;
    hs_interface_ = reinterpret_cast<algorithms::HeuristicSearchStatusInterface<
        GlobalState,
        const ProbabilisticOperator*>*>(i);
}

static PluginTypePlugin<PolicyChooser> _plugin_type("PolicyChooser", "");

} // namespace probabilistic
