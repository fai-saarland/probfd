#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABELS_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABELS_H

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <vector>

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {

/*
  Iterator class for Labels.

  The iterator provides the *index* into label_costs of Labels, which is the
  ID of the label.

  Implementation note: to avoid keeping a reference to label_costs, this class
  stores the current position (curr_pos) in addition to the iterator (it) which
  are always incremented in parallel.
*/
class LabelsConstIterator {
    const std::vector<value_t>::const_iterator end_it;
    std::vector<value_t>::const_iterator it;
    std::size_t current_pos;

    void advance_to_next_valid_index();

public:
    LabelsConstIterator(
        const std::vector<value_t>& label_costs,
        std::vector<value_t>::const_iterator it);
    LabelsConstIterator& operator++();

    int operator*() const { return static_cast<int>(current_pos); }

    friend bool
    operator==(const LabelsConstIterator& lhs, const LabelsConstIterator& rhs)
    {
        return lhs.it == rhs.it;
    }
};

/*
  This class serves both as a container class to handle the set of all
  labels and to perform label reduction on this set.

  Labels are identified via integers indexing label_costs, which stores their
  costs. When using label reductions, labels that become inactive are set to
  -1 in label_costs.
*/
class Labels {
    std::vector<value_t> label_costs;
    int max_num_labels;    // The maximum number of labels that can be created.
    int num_active_labels; // The current number of active (non-reduced) labels.

public:
    Labels(std::vector<value_t>&& label_costs, int max_num_labels);

    value_t get_label_cost(int label) const;
    void dump_labels(utils::LogProxy log) const;

    // The summed number of both inactive and active labels.
    int get_num_total_labels() const { return label_costs.size(); }

    int get_max_num_labels() const { return max_num_labels; }

    int get_num_active_labels() const { return num_active_labels; }

    void reduce_labels(const std::vector<int>& old_labels);

    LabelsConstIterator begin() const
    {
        return LabelsConstIterator(label_costs, label_costs.begin());
    }

    LabelsConstIterator end() const
    {
        return LabelsConstIterator(label_costs, label_costs.end());
    }
};

} // namespace probfd::merge_and_shrink

#endif
