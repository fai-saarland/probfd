#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw.h"

#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd::cartesian_abstractions {
class AbstractState;
class CartesianAbstraction;
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/**
 * @brief Find flaws in the abstraction.
 */
class FlawGenerator {
public:
    virtual ~FlawGenerator() = default;

    virtual std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskTuple& task,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        downward::utils::LogProxy& log,
        downward::utils::CountdownTimer& timer) = 0;

    virtual void notify_split() = 0;

    virtual void print_statistics(downward::utils::LogProxy& log) = 0;
};

class FlawGeneratorFactory {
public:
    virtual ~FlawGeneratorFactory() = default;
    virtual std::unique_ptr<FlawGenerator> create_flaw_generator() = 0;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_H
