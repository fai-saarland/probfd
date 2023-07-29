#ifndef PROBFD_HEURISTICS_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

#include "downward/utils/timer.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

class TraceGenerator;

/*
  Find flaws in a generated trace.
*/
class TraceBasedFlawGenerator : public FlawGenerator {
    std::unique_ptr<TraceGenerator> trace_generator;

    utils::Timer find_trace_timer = utils::Timer(true);
    utils::Timer find_flaw_timer = utils::Timer(true);

    std::unique_ptr<Trace> find_trace(
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        const Trace& solution,
        Abstraction& abstraction,
        utils::LogProxy& log,
        utils::CountdownTimer& timer);

public:
    TraceBasedFlawGenerator(TraceGenerator* trace_generator);
    ~TraceBasedFlawGenerator();

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) override;

    void notify_split() override;

    void print_statistics(utils::LogProxy& log) override;
};

class AStarFlawGeneratorFactory : public FlawGeneratorFactory {
public:
    std::unique_ptr<FlawGenerator> create_flaw_generator() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
