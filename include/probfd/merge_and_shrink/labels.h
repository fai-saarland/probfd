#ifndef PROBFD_MERGE_AND_SHRINK_LABELS_H
#define PROBFD_MERGE_AND_SHRINK_LABELS_H

#include "probfd/value_type.h"

#include "probfd/json/json.h"

#include <iosfwd>
#include <memory>
#include <ranges>
#include <vector>

namespace downward {
template <typename>
class OperatorCostFunction;
}

namespace downward::utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticOperatorProxy;
class ProbabilisticOperatorsProxy;
} // namespace probfd

namespace probfd::merge_and_shrink {

struct LabelInfo {
    value_t cost;
    std::vector<value_t> probabilities;

    explicit LabelInfo(const json::JsonObject& object);
    LabelInfo(value_t cost, ProbabilisticOperatorProxy op);
    LabelInfo(value_t cost, std::vector<value_t> probabilities);

    friend std::unique_ptr<json::JsonObject> to_json(const LabelInfo& labels);
};

/*
  This class serves both as a container class to handle the set of all
  labels and to perform label reduction on this set.

  Labels are identified via integers indexing label_costs, which stores their
  costs. When using label reductions, labels that become inactive are set to
  -1 in label_costs.
*/
class Labels {
    std::vector<LabelInfo> label_infos;

    int max_num_labels;    // The maximum number of labels that can be created.
    int num_active_labels; // The current number of active (non-reduced) labels.

public:
    explicit Labels(const json::JsonObject& object);

    explicit Labels(
        ProbabilisticOperatorsProxy operators,
        const downward::OperatorCostFunction<value_t>& cost_function);

    Labels(
        std::vector<LabelInfo> label_infos,
        int max_num_labels,
        int num_active_labels);

    value_t get_label_cost(int label) const;

    const std::vector<value_t>& get_label_probabilities(int label) const;

    // The summed number of both inactive and active labels.
    int get_num_total_labels() const { return label_infos.size(); }

    int get_max_num_labels() const { return max_num_labels; }

    int get_num_active_labels() const { return num_active_labels; }

    void reduce_labels(const std::vector<int>& old_labels);

    auto get_active_labels() const
    {
        using namespace std::views;
        return zip(iota(0U, label_infos.size()), label_infos) |
               filter(
                   [](const auto& p) { return std::get<1>(p).cost != -1_vt; });
    }

    void dump_labels(downward::utils::LogProxy log) const;

    friend std::unique_ptr<json::JsonObject> to_json(const Labels& labels);
};

} // namespace probfd::merge_and_shrink

#endif
