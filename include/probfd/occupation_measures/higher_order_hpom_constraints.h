#ifndef PROBFD_OCCUPATION_MEASURES_HIGHER_ORDER_HPOM_CONSTRAINTS_H
#define PROBFD_OCCUPATION_MEASURES_HIGHER_ORDER_HPOM_CONSTRAINTS_H

#include "probfd/occupation_measures/constraint_generator.h"

#include <cstddef>
#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class VariablesProxy;
}

/// Namespace dedicated to occupation measure heuristics
namespace probfd::occupation_measures {

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

        [[nodiscard]]
        int get_state_id(const std::vector<int>& state) const;

        [[nodiscard]]
        int get_updated_id(
            const std::vector<int>& pattern,
            const std::vector<int>& state,
            const std::vector<int>& pstate) const;

        [[nodiscard]]
        int to_id(const std::vector<int>& pattern, const downward::State& state)
            const;
    };

    std::vector<PatternInfo> infos_;

public:
    explicit HigherOrderHPOMGenerator(int projection_size);

    void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        downward::lp::LinearProgram& lp) final;

    void update_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) final;

    void reset_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) final;

private:
    [[nodiscard]]
    std::vector<int> get_first_pattern() const;
};

class HigherOrderHPOMGeneratorFactory : public ConstraintGeneratorFactory {
    const int projection_size_;

public:
    explicit HigherOrderHPOMGeneratorFactory(int projection_size);

    std::unique_ptr<ConstraintGenerator> construct_constraint_generator(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override;
};

} // namespace probfd::occupation_measures

#endif // PROBFD_OCCUPATION_MEASURES_HIGHER_ORDER_HPOM_CONSTRAINTS_H
