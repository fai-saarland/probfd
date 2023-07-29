#ifndef PROBFD_HEURISTICS_CARTESIAN_CEGAR_H
#define PROBFD_HEURISTICS_CARTESIAN_CEGAR_H

#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/flaw_generator.h"
#include "probfd/heuristics/cartesian/split_selector.h"

#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"

#include <memory>
#include <optional>

namespace utils {
class RandomNumberGenerator;
class LogProxy;
class Timer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {

class Abstraction;

/*
  Iteratively refine a Cartesian abstraction with counterexample-guided
  abstraction refinement (CEGAR).

  Store the abstraction, use FlawGenerator to find flaws, use SplitSelector to
  select splits in case of ambiguities and break spurious solutions.
*/
class CEGAR {
    const ProbabilisticTaskProxy task_proxy;
    const std::vector<int> domain_sizes;
    const int max_states;
    const int max_non_looping_transitions;
    const SplitSelector split_selector;

    std::unique_ptr<Abstraction> abstraction;
    CartesianCostFunction cost_function;

    std::unique_ptr<FlawGenerator> flaw_generator;

    // Limit the time for building the abstraction.
    utils::CountdownTimer timer;

    utils::LogProxy& log;

    bool may_keep_refining() const;

    /*
      Map all states that can only be reached after reaching the goal
      fact to arbitrary goal states.

      We need this method only for landmark subtasks, but calling it
      for other subtasks with a single goal fact doesn't hurt and
      simplifies the implementation.
    */
    void separate_facts_unreachable_before_goal();

    void refine_abstraction(
        const Flaw& flaw,
        utils::RandomNumberGenerator& rng,
        utils::Timer& timer);

    // Build abstraction.
    void refinement_loop(utils::RandomNumberGenerator& rng);

    void print_statistics();

public:
    CEGAR(
        const std::shared_ptr<ProbabilisticTask>& task,
        const FlawGeneratorFactory& flaw_generator_factory,
        int max_states,
        int max_non_looping_transitions,
        double max_time,
        PickSplit pick,
        utils::RandomNumberGenerator& rng,
        utils::LogProxy& log);
    ~CEGAR();

    CEGAR(const CEGAR&) = delete;

    std::unique_ptr<Abstraction> extract_abstraction();

    CartesianHeuristic& get_heuristic();
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
