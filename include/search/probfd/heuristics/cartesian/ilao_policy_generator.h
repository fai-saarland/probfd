#ifndef PROBFD_HEURISTICS_CARTESIAN_ILAO_POLICY_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ILAO_POLICY_GENERATOR_H

#include "probfd/heuristics/cartesian/policy_generator.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/progress_report.h"

#include <memory>

namespace probfd {

namespace quotients {
template <typename, typename>
struct QuotientState;
template <typename>
struct QuotientAction;
} // namespace quotients

namespace heuristics {
namespace cartesian {

struct ProbabilisticTransition;

/**
 * @brief Find an optimal policy using ILAO*.
 */
class ILAOPolicyGenerator : public PolicyGenerator {
    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<
        quotients::QuotientState<int, const ProbabilisticTransition*>,
        quotients::QuotientAction<const ProbabilisticTransition*>>>
        picker;

    ProgressReport report;

public:
    ILAOPolicyGenerator();

    std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& time_limit) override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif