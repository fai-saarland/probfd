#include "probfd/successor_sorting/hbased_sorter.h"

#include "probfd/new_state_handlers/store_heuristic.h"

#include "option_parser.h"
#include "plugin.h"

#include <algorithm>

namespace probfd {
namespace successor_sorting {

HBasedSorter::HBasedSorter(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void HBasedSorter::add_options_to_parser(options::OptionParser& p)
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

void HBasedSorter::sort(
    const StateID&,
    const std::vector<const ProbabilisticOperator*>&,
    std::vector<Distribution<StateID>>& successors)
{
    std::vector<SortKey> vals(successors.size());
    std::vector<std::pair<int, unsigned>> hests;
    for (int i = successors.size() - 1; i >= 0; --i) {
        std::vector<WeightedElement<StateID>>& t = successors[i].data();
        auto& val = vals[i];
        val.index = i;
        for (unsigned j = 0; j < t.size(); ++j) {
            const auto& [element, probability] = t[j];
            const int h = heuristic_->get_cached_h_value(element);
            hests.emplace_back(h, j);
            if (h < 0) {
                val.dead_end += probability;
            } else {
                val.exp_h += probability * h;
            }
        }
        std::sort(hests.begin(), hests.end());
        std::vector<WeightedElement<StateID>> news;
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

static Plugin<ProbabilisticOperatorSuccessorSorting> _plugin(
    "hbased_sort",
    options::parse<ProbabilisticOperatorSuccessorSorting, HBasedSorter>);

} // namespace successor_sorting
} // namespace probfd
