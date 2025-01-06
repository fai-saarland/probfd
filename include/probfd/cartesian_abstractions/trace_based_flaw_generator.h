#ifndef PROBFD_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "downward/utils/timer.h"

#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
namespace utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd::cartesian_abstractions {
class AbstractState;
class CartesianAbstraction;
class CartesianHeuristic;
class TraceGenerator;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/*
  Find flaws in a generated trace.
*/
class TraceBasedFlawGenerator : public FlawGenerator {
    std::unique_ptr<TraceGenerator> trace_generator_;

    utils::Timer find_trace_timer_ = utils::Timer(true);
    utils::Timer find_flaw_timer_ = utils::Timer(true);

    std::unique_ptr<Trace> find_trace(
        CartesianAbstraction& abstraction,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        const Trace& solution,
        CartesianAbstraction& abstraction,
        utils::LogProxy& log,
        utils::CountdownTimer& timer);

public:
    explicit TraceBasedFlawGenerator(TraceGenerator* trace_generator);
    ~TraceBasedFlawGenerator() override;

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

class AStarFlawGeneratorFactory : public FlawGeneratorFactory {
public:
    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H
