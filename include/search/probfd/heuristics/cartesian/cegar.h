#ifndef PROBFD_HEURISTICS_CARTESIAN_CEGAR_H
#define PROBFD_HEURISTICS_CARTESIAN_CEGAR_H

#include "probfd/heuristics/cartesian/types.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

namespace utils {
class Timer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {

class AbstractState;
class Abstraction;
class CartesianAbstractionInfo;
class CartesianHeuristic;
struct Flaw;
class FlawGenerator;
class FlawGeneratorFactory;
class SplitSelector;
class SplitSelectorFactory;

/*
  Iteratively refine a Cartesian abstraction with counterexample-guided
  abstraction refinement (CEGAR).

  Computes the abstraction, uses FlawGenerator to find flaws, uses SplitSelector
  to select splits in case of ambiguities and break spurious solutions.
*/
class CEGAR {
    const int max_states;
    const int max_non_looping_transitions;
    const std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory;
    const std::shared_ptr<SplitSelectorFactory> split_selector_factory;

    mutable utils::LogProxy log;

public:
    CEGAR(
        int max_states,
        int max_non_looping_transitions,
        std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
        std::shared_ptr<SplitSelectorFactory> split_selector_factory,
        utils::LogProxy log);

    ~CEGAR();

    // Build abstraction.
    CartesianAbstractionInfo run_refinement_loop(
        const std::shared_ptr<ProbabilisticTask>& task,
        double max_time);

private:
    bool may_keep_refining(const Abstraction& abstraction) const;

    /*
        Map all states that can only be reached after reaching the goal
        fact to arbitrary goal states.

        We need this method only for landmark subtasks, but calling it
        for other subtasks with a single goal fact doesn't hurt and
        simplifies the implementation.
    */
    bool separate_facts_unreachable_before_goal(
        ProbabilisticTaskProxy task_proxy,
        FlawGenerator& flaw_generator,
        RefinementHierarchy& refinement_hierarchy,
        Abstraction& abstraction,
        CartesianHeuristic& heuristic,
        utils::Timer& timer);

    bool refine_abstraction(
        FlawGenerator& flaw_generator,
        SplitSelector& split_selector,
        RefinementHierarchy& refinement_hierarchy,
        Abstraction& abstraction,
        CartesianHeuristic& heuristic,
        const Flaw& flaw,
        utils::Timer& timer);

    bool refine_abstraction(
        FlawGenerator& flaw_generator,
        RefinementHierarchy& refinement_hierarchy,
        Abstraction& abstraction,
        CartesianHeuristic& heuristic,
        const AbstractState& abstract_state,
        int split_var,
        const std::vector<int>& wanted);
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
