#include "probfd/merge_and_shrink/labels.h"

#include "probfd/algorithms/types.h"

#include "probfd/task_proxy.h"

#include "probfd/utils/json.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <ranges>

using namespace std;

namespace probfd::merge_and_shrink {

LabelInfo::LabelInfo(const json::JsonObject& object)
    : cost(object.read<value_t>("cost"))
    , probabilities(object.read<std::vector<value_t>>("probabilities"))
{
}

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

std::unique_ptr<json::JsonObject> to_json(const LabelInfo& label_info)
{
    return json::make_object(
        "cost",
        label_info.cost,
        "probabilities",
        label_info.probabilities);
}

Labels::Labels(const json::JsonObject& object)
    : label_infos(object.read<std::vector<LabelInfo>>("label_infos"))
    , max_num_labels(object.read<int>("max_num_labels"))
    , num_active_labels(object.read<int>("num_active_labels"))
{
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
    auto& [first_label_cost, new_label_probabilities] =
        label_infos[old_labels.front()];

    value_t new_label_cost = first_label_cost;
    first_label_cost = -1_vt;

    for (const int old_label : std::views::drop(old_labels, 1)) {
        auto& [cost, probabilities] = label_infos[old_label];

        assert(probabilities == new_label_probabilities);

        if (cost < new_label_cost) {
            new_label_cost = cost;
        }

        cost = -1_vt;
        utils::release_vector_memory(probabilities);
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
        if (const auto& [cost, probabilities] = label_infos[label];
            cost != -1_vt) {
            log << "label " << label << ", cost " << cost << ", probabilities "
                << probabilities << endl;
        }
    }
}

std::unique_ptr<json::JsonObject> to_json(const Labels& labels)
{
    return json::make_object(
        "label_infos",
        labels.label_infos,
        "max_num_labels",
        labels.max_num_labels,
        "num_active_labels",
        labels.num_active_labels);
}

} // namespace probfd::merge_and_shrink
