#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABELS_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABELS_H

#include "probfd/value_type.h"

#include <memory>
#include <ranges>
#include <vector>

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {

struct LabelInfo {
    value_t cost;
    std::vector<value_t> probabilities;
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
    Labels(std::vector<LabelInfo>&& label_infos, int max_num_labels);

    value_t get_label_cost(int label) const;

    const std::vector<value_t>& get_label_probabilities(int label) const;

    const LabelInfo& get_label_info(int label) const;

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

    void dump_labels(utils::LogProxy log) const;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABELS_H
