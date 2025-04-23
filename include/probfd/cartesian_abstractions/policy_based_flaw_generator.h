#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "probfd/aliases.h"

#include "downward/utils/timer.h"

#include <memory>
#include <optional>
#include <vector>

namespace downward::utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::cartesian_abstractions {
class AbstractState;
class CartesianAbstraction;
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

    downward::utils::Timer find_policy_timer_ = downward::utils::Timer(false);
    downward::utils::Timer find_flaw_timer_ = downward::utils::Timer(false);

    std::unique_ptr<Solution> find_solution(
        CartesianAbstraction& abstraction,
        const AbstractState* init,
        CartesianHeuristic& heuristic,
        downward::utils::CountdownTimer& timer);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTask& task,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        Solution& solution,
        downward::utils::LogProxy& log,
        downward::utils::CountdownTimer& timer);

public:
    PolicyBasedFlawGenerator(
        PolicyGenerator* policy_generator,
        PolicyFlawFinder* policy_flaw_finder);

    ~PolicyBasedFlawGenerator() override;

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTask& task,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        downward::utils::LogProxy& log,
        downward::utils::CountdownTimer& timer) override;

    void notify_split() override;

    void print_statistics(downward::utils::LogProxy& log) override;
};

class ILAOFlawGeneratorFactory : public FlawGeneratorFactory {
    value_t convergence_epsilon_;
    int max_search_states_;

public:
    ILAOFlawGeneratorFactory(
        value_t convergence_epsilon,
        int max_search_states);

    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H
