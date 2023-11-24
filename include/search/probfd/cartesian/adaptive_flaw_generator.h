#ifndef PROBFD_CARTESIAN_ADAPTIVE_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_ADAPTIVE_FLAW_GENERATOR_H

#include "probfd/cartesian/flaw_generator.h"

#include <memory>
#include <vector>

namespace plugins {
class Options;
}

namespace probfd {
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
        const std::vector<int>& domain_sizes,
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) override;

    void notify_split() override;

    void print_statistics(utils::LogProxy& log) override;
};

class AdaptiveFlawGeneratorFactory : public FlawGeneratorFactory {
    std::vector<std::shared_ptr<FlawGeneratorFactory>> generator_factories;

public:
    explicit AdaptiveFlawGeneratorFactory(const plugins::Options& opts);

    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace cartesian
} // namespace probfd

#endif // PROBFD_CARTESIAN_ADAPTIVE_FLAW_GENERATOR_H
