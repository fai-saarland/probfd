#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_TRANSITION_SYSTEM_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_TRANSITION_SYSTEM_H

#include "probfd/merge_and_shrink/transition.h"
#include "probfd/merge_and_shrink/types.h"

#include "probfd/value_type.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <deque>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class Distances;
class Labels;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

using LabelGroup = std::vector<int>;

/*
  Class for representing groups of labels with equivalent transitions in a
  transition system. See also documentation for TransitionSystem.

  The local label is in a consistent state if label_group and transitions
  are sorted and unique.
*/
class LocalLabelInfo {
    // The sorted set of labels with identical transitions in a transition
    // system.
    LabelGroup label_group;

    std::vector<Transition> transitions;

    // The probabilities of each individual effect
    std::vector<value_t> probabilities;

    // The cost is the minimum cost over all labels in label_group.
    value_t cost;

public:
    LocalLabelInfo(
        LabelGroup label_group,
        std::vector<Transition> transitions,
        std::vector<value_t> probabilities,
        value_t cost)
        : label_group(std::move(label_group))
        , transitions(std::move(transitions))
        , probabilities(std::move(probabilities))
        , cost(cost)
    {
        assert(is_consistent());
    }

    static LocalLabelInfo read_from_file(std::istream& file);

    void add_label(int label, value_t label_cost);

    /*
      Remove the labels in old_labels from label_group and add new_label to it.
      Requires old_labels to be sorted.
    */
    void apply_same_cost_label_mapping(
        int new_label,
        const std::vector<int>& old_labels);

    // Remove the labels in old_labels. Requires old_labels to be sorted.
    void remove_labels(const std::vector<int>& old_labels);

    void recompute_cost(const Labels& labels);
    void replace_transitions(std::vector<Transition>&& new_transitions);

    /*
      The given local label must have identical transitions. Its labels are
      moved into this local label info. The given local label is then
      invalidated.
    */
    void merge_local_label_info(LocalLabelInfo& local_label_info);

    // Empty all data structures.
    void deactivate();

    // A local label is active as long as it represents labels (in label_group).
    bool is_active() const { return !label_group.empty(); }

    const LabelGroup& get_label_group() const { return label_group; }

    std::vector<Transition>& get_transitions() { return transitions; }

    const std::vector<Transition>& get_transitions() const
    {
        return transitions;
    }

    std::vector<value_t>& get_probabilities() { return probabilities; }

    const std::vector<value_t>& get_probabilities() const
    {
        return probabilities;
    }

    value_t get_cost() const { return cost; }

    std::size_t get_num_transitions() const { return transitions.size(); }

    bool is_consistent() const;

    friend std::ostream&
    operator<<(std::ostream& out, const LocalLabelInfo& label_info);

    friend void
    dump_to_file(std::ostream& out, const LocalLabelInfo& label_info);

    friend void dump_json(std::ostream& os, const LocalLabelInfo& info);
    static LocalLabelInfo read_json(std::istream& is);

    friend std::partial_ordering compare_transitions(
        const LocalLabelInfo& left,
        const LocalLabelInfo& right);

    void merge(const LocalLabelInfo& right);
};

void dump_json(std::ostream& os, const LocalLabelInfo& info);

std::partial_ordering
compare_transitions(const LocalLabelInfo& left, const LocalLabelInfo& right);

class TransitionSystem {
    /*
     * This attribute is only used for output. Vvariables that contributed to
     * this transition system.
     */
    std::vector<int> incorporated_variables;

    /*
     * All locally equivalent labels are grouped together, and their
     * transitions are only stored once for every such group, see below.
     *
     * LabelEquivalenceRelation stores the equivalence relation over all
     * labels of the underlying factored transition system.
     */
    std::vector<int> label_to_local_label;
    std::vector<LocalLabelInfo> local_label_infos;

    int init_state;
    std::vector<bool> goal_states;

    /*
     * Check if two or more labels are locally equivalent to each other, and
     * if so, update the label equivalence relation.
     */
    void compute_equivalent_local_labels();

    // Statistics and output
    int compute_total_transitions() const;

    /*
     * The transitions for every group of locally equivalent labels are
     * sorted (by source, by target) and there are no duplicates.
     */
    bool are_local_labels_consistent() const;

    /*
      The mapping label_to_local_label is consistent with local_label_infos.
    */
    bool is_label_mapping_consistent(const Labels& labels) const;

    void dump_label_mapping(const Labels& labels, std::ostream& out) const;

public:
    TransitionSystem(
        std::vector<int> incorporated_variables,
        std::vector<int> label_to_local_label,
        std::vector<LocalLabelInfo> local_label_infos,
        int init_state,
        std::vector<bool> goal_states);

    int get_size() const { return static_cast<int>(goal_states.size()); }

    int get_init_state() const { return init_state; }

    bool is_goal_state(int state) const { return goal_states[state]; }

    const std::vector<int>& get_incorporated_variables() const
    {
        return incorporated_variables;
    }

    auto label_infos() const
    {
        using namespace std::views;
        return local_label_infos | filter(&LocalLabelInfo::is_active);
    }

    /*
      Factory method to construct the merge of two transition systems.

      Invariant: the children ts1 and ts2 must be solvable.
      (It is a bug to merge an unsolvable transition system.)
    */
    static std::unique_ptr<TransitionSystem> merge(
        const Labels& labels,
        const TransitionSystem& ts1,
        const TransitionSystem& ts2,
        utils::LogProxy& log);

    /*
      Applies the given state equivalence relation to the transition system.
      abstraction_mapping is a mapping from old states to new states, and it
      must be consistent with state_equivalence_relation in the sense that
      old states are only mapped to the same new state if they are in the
      same equivalence class as specified in state_equivalence_relation.
    */
    void apply_abstraction(
        const StateEquivalenceRelation& state_equivalence_relation,
        const std::vector<int>& abstraction_mapping,
        utils::LogProxy& log);

    /*
      Applies the given label mapping, mapping old to new label numbers.
      This updates the label equivalence relation which is internally used
      to group locally equivalent labels and store their transitions only
      once.
    */
    void apply_label_reduction(
        const Labels& labels,
        const std::vector<std::pair<int, std::vector<int>>>& label_mapping,
        bool only_equivalent_labels);

    /*
      The transitions for every group of locally equivalent labels are
      sorted (by source, by target) and there are no duplicates.
    */
    bool is_valid(const Labels& labels) const;

    bool is_solvable(const Distances& distances) const;

    /*
      Method to identify the transition system in output.
      Print "Atomic transition system #x: " for atomic transition systems,
      where x is the variable. For composite transition systems, print
      "Transition system (x/y): " for a transition system containing x
      out of y variables.
    */
    std::string tag() const;

    void dump_statistics(utils::LogProxy& log) const;
    void dump_dot_graph(utils::LogProxy& log) const;
    void dump_labels_and_transitions(utils::LogProxy& log) const;

    void dump_to_file(std::ostream& out) const;

    static TransitionSystem read_from_file(std::istream& is);

    friend std::ostream&
    operator<<(std::ostream& os, const TransitionSystem& ts);

    friend bool
    operator==(const TransitionSystem& left, const TransitionSystem& right) =
        default;

    friend void dump_json(std::ostream& os, const TransitionSystem& ts);
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_TRANSITION_SYSTEM_H
