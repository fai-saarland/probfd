#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABEL_REDUCTION_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_LABEL_REDUCTION_H

#include <memory>
#include <vector>

namespace equivalence_relation {
class EquivalenceRelation;
}

namespace plugins {
class Options;
}

namespace utils {
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
    // Options for label reduction
    std::vector<int> transition_system_order;
    bool lr_before_shrinking;
    bool lr_before_merging;
    LabelReductionMethod lr_method;
    LabelReductionSystemOrder lr_system_order;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    bool initialized() const;
    /* Apply the given label equivalence relation to the set of labels and
       compute the resulting label mapping. */
    void compute_label_mapping(
        const equivalence_relation::EquivalenceRelation& relation,
        const FactoredTransitionSystem& fts,
        std::vector<std::pair<int, std::vector<int>>>& label_mapping,
        utils::LogProxy& log) const;
    equivalence_relation::EquivalenceRelation
    compute_combinable_equivalence_relation(
        int ts_index,
        const FactoredTransitionSystem& fts) const;

public:
    explicit LabelReduction(const plugins::Options& options);

    void initialize(const ProbabilisticTaskProxy& task_proxy);
    bool reduce(
        int merge_index_left,
        int merge_index_right,
        FactoredTransitionSystem& fts,
        utils::LogProxy& log) const;
    void dump_options(utils::LogProxy& log) const;
    bool reduce_before_shrinking() const { return lr_before_shrinking; }
    bool reduce_before_merging() const { return lr_before_merging; }
};

} // namespace probfd::merge_and_shrink

#endif
