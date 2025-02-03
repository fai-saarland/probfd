#ifndef PROBFD_SOLVERS_MDP_SOLVER_H
#define PROBFD_SOLVERS_MDP_SOLVER_H

#include "probfd/solver_interface.h" // IWYU pragma: export

#include "probfd/fdr_types.h"
#include "probfd/progress_report.h"
#include "probfd/task_proxy.h"
#include "probfd/task_state_space.h"

#include "downward/utils/logging.h"

#include <memory>
#include <string>

// Forward Declarations
namespace probfd {
class ProbabilisticTask;
class TaskHeuristicFactory;
class TaskStateSpaceFactory;
} // namespace probfd

/// This namespace contains the solver interface base class for various search
/// algorithms.
namespace probfd::solvers {

/**
 * @brief Base interface for MDP solvers.
 */
class MDPSolver : public TaskSolverFactory {
    mutable utils::LogProxy log_;

private:
    const std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory_;
    const std::shared_ptr<TaskHeuristicFactory> heuristic_factory_;

    const std::string policy_filename;
    const bool print_fact_names;

    const std::optional<value_t> report_epsilon;
    const bool report_enabled;

public:
    /**
     * @brief Constructs the MDP solver from the given arguments.
     */
    MDPSolver(
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    ~MDPSolver() override;

    /**
     * @brief Factory method a new instance of the encapsulated MDP algorithm.
     */
    virtual std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) = 0;

    /**
     * @brief Returns the name of the encapsulated MDP algorithm.
     */
    virtual std::string get_algorithm_name() const = 0;

    std::unique_ptr<SolverInterface>
    create(const std::shared_ptr<ProbabilisticTask>& task) override;
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_MDP_SOLVER_H
