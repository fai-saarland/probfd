#ifndef PROBFD_MERGE_AND_SHRINK_FACTORED_TRANSITION_SYSTEM_H
#define PROBFD_MERGE_AND_SHRINK_FACTORED_TRANSITION_SYSTEM_H

#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/types.h"

#include "downward/utils/logging.h"

#include <vector>

namespace downward::utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class Distances;
class FactoredTransitionSystem;
class FactoredMapping;
class TransitionSystem;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class FTSConstIterator {
    /*
      This class allows users to easily iterate over the active indices of
      a factored transition system.
    */
    const FactoredTransitionSystem& fts;
    // current_index is the actual iterator
    int current_index;

    void next_valid_index();

public:
    FTSConstIterator(const FactoredTransitionSystem& fts, bool end);
    void operator++();

    int operator*() const { return current_index; }

    friend bool
    operator==(const FTSConstIterator& lhs, const FTSConstIterator& rhs)
    {
        return lhs.current_index == rhs.current_index;
    }
};

struct Factor {
    std::unique_ptr<TransitionSystem> transition_system;
    std::unique_ptr<FactoredMapping> factored_mapping;
    std::unique_ptr<Distances> distances;

    Factor() = default;

    Factor(Factor&&) noexcept;
    Factor& operator=(Factor&&) noexcept;
    ~Factor();

    bool is_valid() const;
};

/*
  NOTE: A "factor" of this factored transition system is identfied by its
  index as used in the vectors in this class. Since transformations like
  merging also add and remove factors, not all indices are necessarily
  associated with factors. This is what the class uses the notion of "active"
  factors for: an index is active iff there exists a transition system, a
  merge-and-shrink representation and an distances object in the corresponding
  vectors.

  TODO: The user of this class has to care more about the notion of active
  factors as we would like it to be. We should change this and clean up the
  interface that this class shows to the outside world.
*/
class FactoredTransitionSystem {
public:
    struct MergeResult {
        Factor left_factor;
        Factor right_factor;
        Factor& merged_factor;
        int merge_index;
    };

private:
    Labels labels;
    std::vector<Factor> factors;
    int num_active_entries;

public:
    FactoredTransitionSystem(Labels labels, std::vector<Factor>&& factors);

    // Merge-and-shrink transformations.
    /*
      Apply the given label mapping to the factored transition system by
      updating all transitions of all transition systems. Only for the factor
      at combinable_index, the local equivalence relation over labels must be
      recomputed; for all factors, all labels that are combined by the label
      mapping have been locally equivalent already before.
    */
    void apply_label_mapping(
        const std::vector<std::pair<int, std::vector<int>>>& label_mapping,
        int combinable_index);

    /*
      Apply the given state equivalence relation to the transition system at
      index if it would reduce its size. If the transition system was shrunk,
      update the other components of the factor (distances, MSR) and return
      true, otherwise return false.

      Note that this method is also suitable to be used for a prune
      transformation. All states not mentioned in the state equivalence
      relation are pruned.
    */
    bool apply_abstraction(
        int index,
        const StateEquivalenceRelation& state_equivalence_relation,
        downward::utils::LogProxy& log);

    /*
      Merge the two factors at index1 and index2.
    */
    MergeResult merge(int index1, int index2, downward::utils::LogProxy& log);

    /*
      Extract the factor at the given index, rendering the FTS invalid.
    */
    Factor extract_factor(int index);

    const TransitionSystem& get_transition_system(int index) const
    {
        return *factors[index].transition_system;
    }

    Distances& get_distances(int index) { return *factors[index].distances; }

    const Distances& get_distances(int index) const
    {
        return *factors[index].distances;
    }

    int get_num_active_entries() const { return num_active_entries; }

    // Used by LabelReduction and MergeScoringFunctionDFP
    const Labels& get_labels() const { return labels; }

    int get_size() const { return factors.size(); }

    /*
      A factor is solvable iff the distance of the initial state to some goal
      state is not infinity. Technically, the distance is infinity either if
      the information of Distances is infinity or if the initial state is
      pruned.
    */
    bool is_factor_solvable(int index) const;

    /*
      A factor is trivial iff every concrete state is mapped to an abstract
      goal state, which is equivalent to saying that the corresponding
      merge-and-shrink representation is a total function and all abstract
      states are goal states.

      If h is the heuristic for the factor F, then we have:
          F trivial => h(s) = 0 for all states s.

      Note that a factor being trivial is sufficient but not necessary for
      its heuristic to be useless. Scenarios of useless heuristics that are
      not captured include:
        - All non-goal states are connected to goal states on 0-cost paths.
        - The only pruned states are unreachable (in this case, we get
          h(s) = 0 for all reachable states, which is useless in most
          contexts).
    */
    bool is_factor_trivial(int index) const;

    bool is_active(int index) const;

    // The following methods are used for iterating over the FTS
    FTSConstIterator begin() const { return FTSConstIterator(*this, false); }
    FTSConstIterator end() const { return FTSConstIterator(*this, true); }

    void statistics(int index, downward::utils::LogProxy& log) const;
    void dump(int index, downward::utils::LogProxy& log) const;
    void dump(downward::utils::LogProxy& log) const;

private:
    /*
      Assert that the factor at the given index is in a consistent state, i.e.
      that there is a transition system, a distances object, and an MSR.
    */
    void assert_index_valid(int index) const;

    /*
      We maintain the invariant that for all factors, distances are always
      computed and all transitions are grouped according to locally equivalent
      labels.
    */
    bool is_component_valid(int index) const;

    bool is_factor_valid(const Factor& factor) const;

    void assert_all_components_valid() const;
};

} // namespace probfd::merge_and_shrink

#endif
