#ifndef PROBFD_MERGE_AND_SHRINK_FTS_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_FTS_FACTORY_H

/*
  Factory for factored transition systems.

  Takes a probabilistic planning task and produces a factored transition system
  that represents the planning task. This provides the main bridge from planning
  tasks to the concepts on which merge-and-shrink abstractions are based
  (transition systems, labels, etc.). The "internal" classes of merge-and-shrink
  should not need to know about planning task concepts.
*/

namespace utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
}

namespace probfd::merge_and_shrink {

extern FactoredTransitionSystem create_factored_transition_system(
    const ProbabilisticTaskProxy& task_proxy,
    utils::LogProxy& log);

} // namespace probfd::merge_and_shrink

#endif
