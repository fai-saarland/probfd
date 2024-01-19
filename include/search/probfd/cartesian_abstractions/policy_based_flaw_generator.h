#ifndef PROBFD_CARTESIAN_POLICY_BASED_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_POLICY_BASED_FLAW_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "downward/utils/timer.h"

#include <memory>
#include <optional>
#include <vector>

namespace utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::cartesian_abstractions {
class AbstractState;
class Abstraction;
class CartesianHeuristic;
class PolicyGenerator;
class PolicyFlawFinder;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/**
 * @brief Find flaws using ILAO*.
 */
class PolicyBasedFlawGenerator : public FlawGenerator {
    std::unique_ptr<PolicyGenerator> policy_generator_;
    std::unique_ptr<PolicyFlawFinder> policy_flaw_finder_;

    utils::Timer find_policy_timer_ = utils::Timer(true);
    utils::Timer find_flaw_timer_ = utils::Timer(true);

    std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        Abstraction& abstraction,
        Solution& solution,
        utils::LogProxy& log,
        utils::CountdownTimer& timer);

public:
    PolicyBasedFlawGenerator(
        PolicyGenerator* policy_generator,
        PolicyFlawFinder* policy_flaw_finder);

    ~PolicyBasedFlawGenerator() override;

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) override;

    void notify_split() override;

    void print_statistics(utils::LogProxy& log) override;
};

class ILAOFlawGeneratorFactory : public FlawGeneratorFactory {
    int max_search_states_;

public:
    explicit ILAOFlawGeneratorFactory(int max_search_states);

    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_POLICY_BASED_FLAW_GENERATOR_H
