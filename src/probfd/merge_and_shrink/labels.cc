#include "../../../include/probfd/merge_and_shrink/labels.h"

#include "../../../include/probfd/algorithms/types.h"

#include "../../../include/downward/utils/logging.h"
#include "../../../include/downward/utils/memory.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace probfd::merge_and_shrink {

LabelsConstIterator::LabelsConstIterator(
    const vector<value_t>& label_costs,
    vector<value_t>::const_iterator it)
    : end_it(label_costs.end())
    , it(it)
    , current_pos(distance(label_costs.begin(), it))
{
    advance_to_next_valid_index();
}

void LabelsConstIterator::advance_to_next_valid_index()
{
    while (it != end_it && *it == -1) {
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

Labels::Labels(vector<value_t>&& label_costs, int max_num_labels)
    : label_costs(std::move(label_costs))
    , max_num_labels(max_num_labels)
    , num_active_labels(this->label_costs.size())
{
}

void Labels::reduce_labels(const vector<int>& old_labels)
{
    /*
      Even though we currently only support exact label reductions where
      reduced labels are of equal cost, to support non-exact label reductions,
      we compute the cost of the new label as the minimum cost of all old
      labels reduced to it to satisfy admissibility.
    */
    value_t new_label_cost = INFINITE_VALUE;
    for (int old_label : old_labels) {
        value_t cost = get_label_cost(old_label);
        if (cost < new_label_cost) {
            new_label_cost = cost;
        }
        label_costs[old_label] = -1;
    }
    label_costs.push_back(new_label_cost);
    num_active_labels -= old_labels.size();
    ++num_active_labels;
}

value_t Labels::get_label_cost(int label) const
{
    assert(label_costs[label] != -1);
    return label_costs[label];
}

void Labels::dump_labels(utils::LogProxy log) const
{
    log << "active labels:" << endl;
    for (size_t label = 0; label < label_costs.size(); ++label) {
        if (label_costs[label] != -1) {
            log << "label " << label << ", cost " << label_costs[label] << endl;
        }
    }
}

} // namespace probfd::merge_and_shrink