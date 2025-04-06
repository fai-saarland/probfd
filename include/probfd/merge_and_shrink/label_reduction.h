#ifndef PROBFD_MERGE_AND_SHRINK_LABEL_REDUCTION_H
#define PROBFD_MERGE_AND_SHRINK_LABEL_REDUCTION_H

#include <memory>
#include <vector>

namespace downward::equivalence_relation {
class EquivalenceRelation;
}

namespace downward::utils {
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
}

namespace probfd::merge_and_shrink {

/*
  two_transition_systems: compute the 'combinable relation'
  for labels only for the two transition_systems that will
  be merged next and reduce labels.

  all_transition_systems: compute the 'combinable relation'
  for labels once for every transition_system and reduce
  labels.

  all_transition_systems_with_fixpoint: keep computing the
  'combinable relation' for labels iteratively for all
  transition systems until no more labels can be reduced.
*/
enum class LabelReductionMethod {
    TWO_TRANSITION_SYSTEMS,
    ALL_TRANSITION_SYSTEMS,
    ALL_TRANSITION_SYSTEMS_WITH_FIXPOINT
};
/*
  Order in which iterations of label reduction considers the set of all
  transition systems. Regular is the fast downward order plus appending
  new composite transition systems after the atomic ones, reverse is the
  reversed regular order and random is a random one. All orders are
  precomputed and reused for every call to reduce().
*/
enum class LabelReductionSystemOrder { REGULAR, REVERSE, RANDOM };

class LabelReduction {
    std::vector<int> transition_system_order;
    bool lr_before_shrinking;
    bool lr_before_merging;
    LabelReductionMethod lr_method;
    LabelReductionSystemOrder lr_system_order;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;

    bool initialized() const;

public:
    LabelReduction(
        bool before_shrinking,
        bool before_merging,
        LabelReductionMethod method,
        LabelReductionSystemOrder system_order,
        int random_seed);

    void initialize(const ProbabilisticTaskProxy& task_proxy);

    bool reduce(
        int merge_index_left,
        int merge_index_right,
        FactoredTransitionSystem& fts,
        downward::utils::LogProxy& log) const;

    void dump_options(downward::utils::LogProxy& log) const;

    bool reduce_before_shrinking() const { return lr_before_shrinking; }
    bool reduce_before_merging() const { return lr_before_merging; }
};

} // namespace probfd::merge_and_shrink

#endif
