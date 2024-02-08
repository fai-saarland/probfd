#include "probfd/merge_and_shrink/labels.h"

#include "probfd/algorithms/types.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <ranges>

using namespace std;

namespace probfd::merge_and_shrink {

Labels::Labels(vector<LabelInfo>&& label_infos, int max_num_labels)
    : label_infos(std::move(label_infos))
    , max_num_labels(max_num_labels)
    , num_active_labels(this->label_infos.size())
{
}

value_t Labels::get_label_cost(int label) const
{
    assert(label_infos[label].cost != -1_vt);
    return label_infos[label].cost;
}

const std::vector<value_t>& Labels::get_label_probabilities(int label) const
{
    assert(label_infos[label].cost != -1_vt);
    return label_infos[label].probabilities;
}

const LabelInfo& Labels::get_label_info(int label) const
{
    assert(label_infos[label].cost != -1_vt);
    return label_infos[label];
}

void Labels::reduce_labels(const vector<int>& old_labels)
{
    assert(!old_labels.empty());

    /*
      Even though we currently only support exact label reductions where
      reduced labels are of equal cost, to support non-exact label reductions,
      we compute the cost of the new label as the minimum cost of all old
      labels reduced to it to satisfy admissibility.
    */
    auto& [new_label_cost, new_label_probabilities] = label_infos[old_labels.front()];

    for (const int old_label : std::views::drop(old_labels, 1)) {
        auto& [cost, probabilities] = label_infos[old_label];

        assert(probabilities == new_label_probabilities);

        if (cost < new_label_cost) {
            new_label_cost = cost;
        }

        cost = -1_vt;
        probabilities.clear();
    }

    label_infos.emplace_back(
        new_label_cost,
        std::move(new_label_probabilities));
    num_active_labels -= old_labels.size() + 1;
}

void Labels::dump_labels(utils::LogProxy log) const
{
    log << "active labels:" << endl;
    for (size_t label = 0; label < label_infos.size(); ++label) {
        if (const auto& [cost, probabilities] = label_infos[label];
            cost != -1_vt) {
            log << "label " << label << ", cost " << cost
                << ", probabilities " << probabilities << endl;
        }
    }
}

} // namespace probfd::merge_and_shrink
