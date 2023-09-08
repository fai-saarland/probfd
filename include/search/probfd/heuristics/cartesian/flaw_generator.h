#ifndef PROBFD_HEURISTICS_CARTESIAN_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw.h"
#include "probfd/heuristics/cartesian/types.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>
#include <optional>
#include <vector>

namespace utils {
class CountdownTimer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {
class Abstraction;
class CartesianCostFunction;
class CartesianHeuristic;

/**
 * @brief Find flaws in the abstraction.
 */
class FlawGenerator {
public:
    virtual ~FlawGenerator() = default;

    virtual std::optional<Flaw> generate_flaw(
        ProbabilisticTaskProxy task_proxy,
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::LogProxy log,
        utils::CountdownTimer& timer) = 0;

    virtual void notify_split() = 0;

    virtual void print_statistics(utils::LogProxy log) = 0;
};

class FlawGeneratorFactory {
public:
    virtual ~FlawGeneratorFactory() = default;
    virtual std::unique_ptr<FlawGenerator> create_flaw_generator() = 0;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
