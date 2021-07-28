#include "vdiff_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../engines/heuristic_search_state_information.h"

namespace probabilistic {
namespace policy_tiebreaking {

VDiffTiebreaker::VDiffTiebreaker(const options::Options& opts)
    : favor_large_gaps_(opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

void VDiffTiebreaker::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

int VDiffTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<const ProbabilisticOperator*>&,
    const std::vector<Distribution<StateID>>& successors)
{
    value_type::value_t best = value_type::inf;
    unsigned choice = 1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const Distribution<StateID>& t = successors[i];
        value_type::value_t sum = value_type::zero;
        for (auto it = t.begin(); it != t.end(); ++it) {
            sum += it->second * lookup_dual_bounds(it->first)->error_bound();
        }
        if (value_type::approx_less()(favor_large_gaps_ * sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

VDiffSort::VDiffSort(const options::Options& opts)
    : favor_large_gaps_(opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

void VDiffSort::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

void VDiffSort::sort(
    const StateID&,
    const std::vector<const ProbabilisticOperator*>&,
    std::vector<Distribution<StateID>>& successors)
{
    std::vector<std::pair<double, unsigned>> k0;
    k0.reserve(successors.size());
    std::vector<std::pair<double, unsigned>> k1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const std::vector<std::pair<StateID, value_type::value_t>>& t =
            successors[i].data();
        value_type::value_t sum = 0;
        for (unsigned j = 0; j < t.size(); ++j) {
            const auto& suc = t[j];
            k1.emplace_back(
                std::abs(
                    favor_large_gaps_ *
                    lookup_dual_bounds(suc.first)->error_bound()),
                j);
            sum += suc.second * k1.back().first;
        }
        k0.emplace_back(sum, i);
        std::sort(k1.begin(), k1.end());
        std::vector<std::pair<StateID, value_type::value_t>> sor;
        sor.reserve(t.size());
        for (unsigned j = 0; j < k1.size(); ++j) {
            sor.push_back(t[k1[j].second]);
        }
        k1.clear();
    }
    std::sort(k0.begin(), k0.end());
    std::vector<Distribution<StateID>> res;
    res.reserve(successors.size());
    for (unsigned i = 0; i < k0.size(); ++i) {
        res.push_back(std::move(successors[k0[i].second]));
    }
    res.swap(successors);
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "value_gap_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, VDiffTiebreaker>);

static Plugin<ProbabilisticOperatorSuccessorSorting> _plugin_sort(
    "value_gap_sort",
    options::parse<ProbabilisticOperatorSuccessorSorting, VDiffSort>);

} // namespace policy_tiebreaking
} // namespace probabilistic
