#include "probfd/successor_sorting/preferred_operators_sorter.h"

#include "probfd/new_state_handlers/store_preferred_operators.h"

#include "option_parser.h"
#include "plugin.h"

#include <algorithm>

namespace probfd {
namespace successor_sorting {

PreferredOperatorsSorter::PreferredOperatorsSorter(const options::Options& opts)
    : pref_ops_(std::dynamic_pointer_cast<
                new_state_handlers::StorePreferredOperators>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void PreferredOperatorsSorter::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

void PreferredOperatorsSorter::sort(
    const StateID& state,
    const std::vector<const ProbabilisticOperator*>& choices,
    std::vector<Distribution<StateID>>& successors)
{
    const new_state_handlers::PrefOpsCacheEntry& e =
        pref_ops_->get_cached_ops(state);
    std::vector<std::pair<bool, unsigned>> preferred;
    preferred.reserve(choices.size());
    for (unsigned i = 0; i < choices.size(); ++i) {
        preferred.emplace_back(!e.contains(choices[i]), i);
    }
    std::sort(preferred.begin(), preferred.end());
    std::vector<Distribution<StateID>> res;
    res.reserve(successors.size());
    for (unsigned i = 0; i < preferred.size(); ++i) {
        res.push_back(std::move(successors[preferred[i].second]));
    }
    successors.swap(res);
}

static Plugin<ProbabilisticOperatorSuccessorSorting> _plugin(
    "preferred_operators_policy_sort",
    options::
        parse<ProbabilisticOperatorSuccessorSorting, PreferredOperatorsSorter>);

} // namespace successor_sorting
} // namespace probfd
