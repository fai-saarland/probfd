#ifndef PROBFD_HEURISTICS_CARTESIAN_CEGAR_H
#define PROBFD_HEURISTICS_CARTESIAN_CEGAR_H

#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/flaw.h"
#include "probfd/heuristics/cartesian/split_selector.h"

#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"

#include <memory>
#include <optional>
#include <vector>

namespace utils {
class RandomNumberGenerator;
class LogProxy;
class Timer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {

class AbstractState;
class Abstraction;
class FlawGenerator;
class FlawGeneratorFactory;

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
    const std::unique_ptr<FlawGenerator> flaw_generator;
    const std::unique_ptr<SplitSelector> split_selector;

    mutable utils::LogProxy log;

    // Limit the time for building the abstraction.
    utils::CountdownTimer timer;

    /* DAG with inner nodes for all split states and leaves for all
       current states. */
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy;
    std::unique_ptr<Abstraction> abstraction;
    CartesianCostFunction cost_function;
    CartesianHeuristic heuristic;

public:
    CEGAR(
        const std::shared_ptr<ProbabilisticTask>& task,
        int max_states,
        int max_non_looping_transitions,
        double max_time,
        const FlawGeneratorFactory& flaw_generator_factory,
        PickSplit pick,
        utils::RandomNumberGenerator& rng,
        utils::LogProxy log);

    ~CEGAR();

    std::unique_ptr<RefinementHierarchy> extract_refinement_hierarchy();
    std::unique_ptr<Abstraction> extract_abstraction();

    CartesianHeuristic& get_heuristic();

private:
    bool may_keep_refining() const;

    /*
        Map all states that can only be reached after reaching the goal
        fact to arbitrary goal states.

        We need this method only for landmark subtasks, but calling it
        for other subtasks with a single goal fact doesn't hurt and
        simplifies the implementation.
    */
    void separate_facts_unreachable_before_goal(utils::Timer& timer);

    void refine_abstraction(const Flaw& flaw, utils::Timer& timer);
    void refine_abstraction(
        const AbstractState& abstract_state,
        int split_var,
        const std::vector<int>& wanted);
    // Build abstraction.
    void refinement_loop();

    void print_statistics();
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
