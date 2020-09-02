#include "preferred_operators_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../new_state_handlers/store_preferred_operators.h"

namespace probabilistic {
namespace policy_tiebreaking {

PreferredOperatorsTiebreaker::PreferredOperatorsTiebreaker(
    const options::Options& opts)
    : pref_ops_(
        std::dynamic_pointer_cast<new_state_handlers::StorePreferredOperators>(
            opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void
PreferredOperatorsTiebreaker::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

unsigned
PreferredOperatorsTiebreaker::choose(
    const GlobalState& s,
    const ProbabilisticOperator* prev,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        choices)
{
    const new_state_handlers::PrefOpsCacheEntry& e =
        pref_ops_->get_cached_ops(s);
    unsigned j = -1;
    for (unsigned i = 0; i < choices.size(); ++i) {
        if (e.contains(choices[i].first)) {
            if (choices[i].first == prev) {
                return i;
            }
            j = std::min(i, j);
        }
    }
    return j;
}

unsigned
PreferredOperatorsTiebreaker::choose(
    const QuotientState&,
    const QuotientAction& prev,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        choices)
{
    unsigned j = -1;
    for (unsigned i = 0; i < choices.size(); ++i) {
        const QuotientAction& a = choices[i].first;
        const new_state_handlers::PrefOpsCacheEntry& e =
            pref_ops_->get_cached_ops(a.first);
        if (e.contains(a.second)) {
            if (a == prev) {
                return i;
            }
            j = std::min(i, j);
        }
    }
    return j;
}

static Plugin<PolicyChooser> _plugin(
    "preferred_operators_tiebreaker",
    options::parse<PolicyChooser, PreferredOperatorsTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
