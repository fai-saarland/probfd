#include "hbased_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../new_state_handlers/store_heuristic.h"

#include <algorithm>

namespace probabilistic {
namespace policy_tiebreaking {

HBasedTiebreaker::HBasedTiebreaker(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void HBasedTiebreaker::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

int HBasedTiebreaker::pick(
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
            const int h = heuristic_->get_cached_h_value(it->first);
            if (h < 0) {
                sum = value_type::inf;
                break;
            }
            sum += it->second * h;
        }
        if (value_type::approx_less()(sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

HBasedSort::HBasedSort(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void HBasedSort::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

struct SortKey {
    double dead_end = 0;
    double exp_h = 0;
    unsigned index = -1;

    bool operator<(const SortKey& other) const
    {
        return dead_end < other.dead_end ||
               (dead_end == other.dead_end && exp_h < other.exp_h);
    }
};

void HBasedSort::sort(
    const StateID&,
    const std::vector<const ProbabilisticOperator*>&,
    std::vector<Distribution<StateID>>& successors)
{
    std::vector<SortKey> vals(successors.size());
    std::vector<std::pair<int, unsigned>> hests;
    for (int i = successors.size() - 1; i >= 0; --i) {
        std::vector<std::pair<StateID, value_type::value_t>>& t =
            successors[i].data();
        auto& val = vals[i];
        val.index = i;
        for (unsigned j = 0; j < t.size(); ++j) {
            const auto& succ = t[j];
            const int h = heuristic_->get_cached_h_value(succ.first);
            hests.emplace_back(h, j);
            if (h < 0) {
                val.dead_end += succ.second;
            } else {
                val.exp_h += succ.second * h;
            }
        }
        std::sort(hests.begin(), hests.end());
        std::vector<std::pair<StateID, value_type::value_t>> news;
        news.reserve(t.size());
        for (unsigned j = 0; j < hests.size(); ++j) {
            news.push_back(t[hests[j].second]);
        }
        successors[i].data() = std::move(news);
        hests.clear();
    }
    std::sort(vals.begin(), vals.end());
    std::vector<Distribution<StateID>> sorted;
    sorted.reserve(successors.size());
    for (unsigned i = 0; i < vals.size(); ++i) {
        sorted.push_back(std::move(successors[vals[i].index]));
    }
    successors.swap(sorted);
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "hbiased_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, HBasedTiebreaker>);

static Plugin<ProbabilisticOperatorSuccessorSorting> _plugin_sort(
    "hbased_sort",
    options::parse<ProbabilisticOperatorSuccessorSorting, HBasedSort>);

} // namespace policy_tiebreaking
} // namespace probabilistic
