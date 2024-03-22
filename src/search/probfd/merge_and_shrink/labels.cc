#include "probfd/merge_and_shrink/labels.h"

#include "probfd/merge_and_shrink/types.h"

#include "probfd/task_proxy.h"

#include "probfd/utils/json.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include <cassert>
#include <iostream>
#include <ranges>

using namespace std;

namespace probfd::merge_and_shrink {

LabelInfo::LabelInfo(ProbabilisticOperatorProxy op)
    : cost(op.get_cost())
{
    for (ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
        probabilities.push_back(outcome.get_probability());
    }
}

LabelInfo::LabelInfo(value_t cost, std::vector<value_t> probabilities)
    : cost(cost)
    , probabilities(std::move(probabilities))
{
}

LabelInfo LabelInfo::read_json(std::istream& is)
{
    return json::
        construct_from_object<LabelInfo, value_t, std::vector<value_t>>(
            is,
            "cost",
            "probabilities");
}

void dump_json(std::ostream& os, const LabelInfo& label_info)
{
    json::write_object(
        os,
        std::forward_as_tuple("cost", label_info.cost),
        std::forward_as_tuple("probabilities", label_info.probabilities));
}

Labels::Labels(
    std::vector<LabelInfo> label_infos,
    int max_num_labels,
    int num_active_labels)
    : label_infos(std::move(label_infos))
    , max_num_labels(max_num_labels)
    , num_active_labels(num_active_labels)
{
}

Labels::Labels(ProbabilisticOperatorsProxy operators)
{
    const int num_ops = operators.size();

    if (num_ops > 0) {
        max_num_labels = 2 * num_ops - 1;
        label_infos.reserve(max_num_labels);
        for (const ProbabilisticOperatorProxy op : operators) {
            label_infos.emplace_back(op);
        }
    } else {
        max_num_labels = 0;
    }

    num_active_labels = num_ops;
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
        utils::release_vector_memory(info.probabilities);
    }
    label_infos.emplace_back(
        new_label_cost,
        std::move(new_label_probabilities));
    num_active_labels -= old_labels.size() - 1;
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

Labels Labels::read_json(std::istream& is)
{
    return json::
        construct_from_object<Labels, std::vector<LabelInfo>, int, int>(
            is,
            "label_infos",
            "max_num_labels",
            "num_active_labels");
}

void dump_json(std::ostream& os, const Labels& labels)
{
    json::write_object(
        os,
        std::forward_as_tuple("label_infos", labels.label_infos),
        std::forward_as_tuple("max_num_labels", labels.max_num_labels),
        std::forward_as_tuple("num_active_labels", labels.num_active_labels));
}

} // namespace probfd::merge_and_shrink
