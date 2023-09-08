#ifndef PROBFD_HEURISTICS_CARTESIAN_ILAO_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ILAO_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

#include "downward/utils/timer.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace cartesian {

class PolicyGenerator;
class PolicyFlawFinder;

/**
 * @brief Find flaws using ILAO*.
 */
class PolicyBasedFlawGenerator : public FlawGenerator {
    std::unique_ptr<PolicyGenerator> policy_generator;
    std::unique_ptr<PolicyFlawFinder> policy_flaw_finder;

    utils::Timer find_policy_timer = utils::Timer(true);
    utils::Timer find_flaw_timer = utils::Timer(true);

    std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer);

    std::optional<Flaw> find_flaw(
        ProbabilisticTaskProxy task_proxy,
        Abstraction& abstraction,
        Solution& solution,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

public:
    PolicyBasedFlawGenerator(
        PolicyGenerator* policy_generator,
        PolicyFlawFinder* policy_flaw_finder);

    ~PolicyBasedFlawGenerator();

    std::optional<Flaw> generate_flaw(
        ProbabilisticTaskProxy task_proxy,
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::LogProxy log,
        utils::CountdownTimer& timer) override;

    void notify_split() override;

    void print_statistics(utils::LogProxy log) override;
};

class ILAOFlawGeneratorFactory : public FlawGeneratorFactory {
    int max_search_states;

public:
    ILAOFlawGeneratorFactory(int max_search_states);

    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
