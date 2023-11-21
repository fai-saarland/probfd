#include "probfd/merge_and_shrink/labels.h"

#include "probfd/merge_and_shrink/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include <cassert>
#include <iostream>
#include <ranges>

using namespace std;

namespace probfd::merge_and_shrink {

LabelsConstIterator::LabelsConstIterator(
    const vector<LabelInfo>& label_infos,
    vector<LabelInfo>::const_iterator it)
    : end_it(label_infos.end())
    , it(it)
    , current_pos(distance(label_infos.begin(), it))
{
    advance_to_next_valid_index();
}

void LabelsConstIterator::advance_to_next_valid_index()
{
    while (it != end_it && it->cost == -1_vt) {
        ++it;
        ++current_pos;
    }
}

LabelsConstIterator& LabelsConstIterator::operator++()
{
    ++it;
    ++current_pos;
    advance_to_next_valid_index();
    return *this;
}

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
    auto& front_info = label_infos[old_labels.front()];
    value_t new_label_cost = front_info.cost;
    auto&& new_label_probabilities = std::move(front_info.probabilities);
    for (int old_label : std::views::drop(old_labels, 1)) {
        LabelInfo& info = label_infos[old_label];

        assert(info.probabilities == new_label_probabilities);

        if (info.cost < new_label_cost) {
            new_label_cost = info.cost;
        }
        info.cost = -1_vt;
        info.probabilities.clear();
    }
    label_infos.emplace_back(
        new_label_cost,
        std::move(new_label_probabilities));
    num_active_labels -= old_labels.size();
    ++num_active_labels;
}

void Labels::dump_labels(utils::LogProxy log) const
{
    log << "active labels:" << endl;
    for (size_t label = 0; label < label_infos.size(); ++label) {
        const auto& label_info = label_infos[label];
        if (label_info.cost != -1_vt) {
            log << "label " << label << ", cost " << label_info.cost
                << ", probabilities " << label_info.probabilities << endl;
        }
    }
}

} // namespace probfd::merge_and_shrink
