#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
namespace utils {
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
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/**
 * @brief A flaw generator that uses an incomplete flaw generator, with a
 * complete flaw generator as a backup.
 */
class AdaptiveFlawGenerator : public FlawGenerator {
    std::vector<std::unique_ptr<FlawGenerator>> generators_;
    size_t current_generator_ = 0;

public:
    explicit AdaptiveFlawGenerator(
        std::vector<std::unique_ptr<FlawGenerator>> generators);

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) override;

    void notify_split() override;

    void print_statistics(utils::LogProxy& log) override;
};

class AdaptiveFlawGeneratorFactory : public FlawGeneratorFactory {
    std::vector<std::shared_ptr<FlawGeneratorFactory>> generator_factories_;

public:
    explicit AdaptiveFlawGeneratorFactory(
        std::vector<std::shared_ptr<FlawGeneratorFactory>> generators);

    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H
