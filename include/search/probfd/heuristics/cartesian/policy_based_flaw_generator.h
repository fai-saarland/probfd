#ifndef PROBFD_HEURISTICS_CARTESIAN_ILAO_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ILAO_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

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

    CartesianHeuristic heuristic;

    bool is_complete() override final;

public:
    PolicyBasedFlawGenerator(
        PolicyGenerator* policy_generator,
        PolicyFlawFinder* policy_flaw_finder);

    ~PolicyBasedFlawGenerator();

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        const AbstractState* init_id,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        utils::Timer& find_trace_timer,
        utils::Timer& find_flaw_timer,
        utils::CountdownTimer& timer,
        int max_search_states) override;

    void notify_split(int v) override;

    CartesianHeuristic& get_heuristic() override;
};

class ILAOFlawGeneratorFactory : public FlawGeneratorFactory {
public:
    std::unique_ptr<FlawGenerator> create_flaw_generator() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
