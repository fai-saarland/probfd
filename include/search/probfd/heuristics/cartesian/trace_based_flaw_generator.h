#ifndef PROBFD_HEURISTICS_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_TRACE_BASED_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

class TraceGenerator;

/*
  Find flaws in a generated trace.
*/
class TraceBasedFlawGenerator : public FlawGenerator {
    std::unique_ptr<TraceGenerator> trace_generator;
    CartesianHeuristic heuristic;

    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const Trace& solution,
        Abstraction& abstraction,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        utils::CountdownTimer& timer);

public:
    TraceBasedFlawGenerator(TraceGenerator* trace_generator);
    ~TraceBasedFlawGenerator();

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

    bool is_complete() override final;
};

class AStarFlawGeneratorFactory : public FlawGeneratorFactory {
public:
    std::unique_ptr<FlawGenerator> create_flaw_generator() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
