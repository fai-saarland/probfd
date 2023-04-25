#ifndef PROBFD_HEURISTICS_CARTESIAN_ILAO_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ILAO_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

namespace utils {
class CountdownTimer;
class Timer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {

/**
 * @brief Find flaws using ILAO*.
 */
class ILAOFlawGenerator : public FlawGenerator {
    CartesianHeuristic heuristic;
    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<
        const AbstractState*,
        const ProbabilisticTransition*>>
        ptb;

    ProgressReport report;

    std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        const AbstractState* init_id,
        utils::CountdownTimer& time_limit);

    /* Try to convert the abstract solution into a concrete trace. Return the
       first encountered flaw or nullptr if there is no flaw. */
    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        Abstraction& abstraction,
        Solution& policy,
        utils::CountdownTimer& timer,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        int max_search_states);

    bool is_complete() override final;

public:
    ILAOFlawGenerator();

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
