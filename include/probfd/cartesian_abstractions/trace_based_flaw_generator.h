#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_BASED_FLAW_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_BASED_FLAW_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "downward/utils/timer.h"

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
class TraceGenerator;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/*
  Find flaws in a generated trace.
*/
class TraceBasedFlawGenerator : public FlawGenerator {
    std::unique_ptr<TraceGenerator> trace_generator_;

    downward::utils::Timer find_trace_timer_ = downward::utils::Timer(false);
    downward::utils::Timer find_flaw_timer_ = downward::utils::Timer(false);

    std::unique_ptr<Trace> find_trace(
        CartesianAbstraction& abstraction,
        int init_id,
        CartesianHeuristic& heuristic,
        downward::utils::CountdownTimer& timer);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTask& task,
        const std::vector<int>& domain_sizes,
        const Trace& solution,
        CartesianAbstraction& abstraction,
        downward::utils::LogProxy& log,
        downward::utils::CountdownTimer& timer);

public:
    explicit TraceBasedFlawGenerator(TraceGenerator* trace_generator);
    ~TraceBasedFlawGenerator() override;

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

class AStarFlawGeneratorFactory : public FlawGeneratorFactory {
public:
    std::unique_ptr<FlawGenerator> create_flaw_generator() override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_BASED_FLAW_GENERATOR_H
