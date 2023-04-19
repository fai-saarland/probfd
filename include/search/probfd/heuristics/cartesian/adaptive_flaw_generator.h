#ifndef PROBFD_HEURISTICS_CARTESIAN_ADAPTIVE_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ADAPTIVE_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

#include <memory>
#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

/**
 * @brief A flaw generator that uses an incomplete flaw generator, with a
 * complete flaw generator as a backup.
 */
class AdaptiveFlawGenerator : public FlawGenerator {
    std::vector<std::unique_ptr<FlawGenerator>> generators;
    size_t current_generator = 0;

public:
    explicit AdaptiveFlawGenerator(
        std::vector<std::unique_ptr<FlawGenerator>> generators);

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        const AbstractState* init_id,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        utils::Timer& find_trace_timer,
        utils::Timer& find_flaw_timer,
        utils::CountdownTimer& timer) override;

    void notify_split(int v) override;

    CartesianHeuristic& get_heuristic() override;

    bool is_complete() override;
};

class AdaptiveFlawGeneratorFactory : public FlawGeneratorFactory {
    std::vector<std::shared_ptr<FlawGeneratorFactory>> generator_factories;

public:
    explicit AdaptiveFlawGeneratorFactory(const options::Options& opts);

    std::unique_ptr<FlawGenerator> create_flaw_generator() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
