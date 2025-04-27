#ifndef PROBFD_SOLVERS_MDP_SOLVER_H
#define PROBFD_SOLVERS_MDP_SOLVER_H

#include "probfd/solver_interface.h" // IWYU pragma: export

#include "probfd/fdr_types.h"
#include "probfd/progress_report.h"
#include "probfd/task_state_space.h"

#include "downward/utils/logging.h"

#include <memory>
#include <string>

// Forward Declarations
namespace probfd {
class TaskHeuristicFactory;
class TaskStateSpaceFactory;

template <typename, typename>
class Policy;
} // namespace probfd

namespace probfd::solvers {
class StatisticalMDPAlgorithmFactory;
}

/// This namespace contains the solver interface base class for various search
/// algorithms.
namespace probfd::solvers {

/**
 * @brief Base interface for MDP solvers.
 */
class MDPSolver : public TaskSolverFactory {
    const std::shared_ptr<StatisticalMDPAlgorithmFactory> algorithm_factory_;
    const std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory_;
    const std::shared_ptr<TaskHeuristicFactory> heuristic_factory_;

    mutable downward::utils::LogProxy log_;

    const std::string policy_filename;
    const bool print_fact_names;

    const std::optional<value_t> report_epsilon;
    const bool report_enabled;

public:
    /**
     * @brief Constructs the MDP solver from the given arguments.
     */
    MDPSolver(
        std::shared_ptr<StatisticalMDPAlgorithmFactory> algorithm_factory,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        downward::utils::Verbosity verbosity,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    ~MDPSolver() override;

    std::unique_ptr<SolverInterface>
    create(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_MDP_SOLVER_H
