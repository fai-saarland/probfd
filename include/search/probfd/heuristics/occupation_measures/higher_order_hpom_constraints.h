#ifndef PROBFD_HEURISTICS_OOCP_HEURISTIC_H
#define PROBFD_HEURISTICS_OOCP_HEURISTIC_H

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "probfd/task_proxy.h"

#include <memory>
#include <vector>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {

namespace heuristics {

/// Namespace dedicated to occupation measure heuristics
namespace occupation_measures {

/**
 * @brief Implements the optimal operator cost partitioning heuristic over a set
 * of PDBs.
 */
class HigherOrderHPOMGenerator : public ConstraintGenerator {
    const int projection_size_;

    struct PatternInfo {
        int offset;
        std::vector<int> multipliers;

        explicit PatternInfo(int offset)
            : offset(offset)
        {
        }

        int get_state_id(const std::vector<int>& state) const;

        int get_updated_id(
            const std::vector<int>& pattern,
            const std::vector<int>& state,
            const std::vector<int>& pstate) const;

        int to_id(const std::vector<int>& pattern, const State& state) const;
    };

    std::vector<PatternInfo> infos_;

public:
    explicit HigherOrderHPOMGenerator(const plugins::Options& opts);

    explicit HigherOrderHPOMGenerator(int projection_size);

    virtual void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<TaskCostFunction>& task_cost_function,
        lp::LinearProgram& lp) override final;

    void
    update_constraints(const State& state, lp::LPSolver& solver) override final;

    void
    reset_constraints(const State& state, lp::LPSolver& solver) override final;

private:
    std::vector<int> get_first_pattern() const;
    bool
    next_pattern(std::size_t num_variables, std::vector<int>& pattern) const;

    std::vector<int> get_first_partial_state(
        const std::vector<int>& pattern,
        const std::vector<int>& indices) const;

    bool next_partial_state(
        const VariablesProxy& variables,
        std::vector<int>& pstate,
        const std::vector<int>& pattern,
        const std::vector<int>& pindices) const;
};

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif // MDPS_HEURISTICS_OOCP_HEURISTIC_H