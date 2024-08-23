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
class TaskCostFunctionFactory;
class TaskEvaluatorFactory;
} // namespace probfd

/// This namespace contains the solver interface base class for various search
/// algorithms.
namespace probfd::solvers {

/**
 * @brief Base interface for MDP solvers.
 */
class MDPSolver : public SolverInterface {
    mutable utils::LogProxy log_;

protected:
    const std::shared_ptr<ProbabilisticTask> task_;

    const std::unique_ptr<TaskStateSpace> task_mdp_;
    const std::shared_ptr<FDRCostFunction> task_cost_function_;

private:
    const std::shared_ptr<TaskEvaluatorFactory> heuristic_factory_;

    ProgressReport progress_;

    const double max_time_;
    const std::string policy_filename;
    const bool print_fact_names;

public:
    /**
     * @brief Constructs the MDP solver from the given arguments.
     */
    MDPSolver(
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
        bool cache,
        std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
        std::optional<value_t> report_epsilon,
        bool report_enabled,
        double max_time,
        std::string policy_filename,
        bool print_fact_names);

    ~MDPSolver() override;

    /**
     * @brief Factory method a new instance of the encapsulated MDP algorithm.
     */
    virtual std::unique_ptr<FDRMDPAlgorithm> create_algorithm() = 0;

    /**
     * @brief Returns the name of the encapsulated MDP algorithm.
     */
    virtual std::string get_algorithm_name() const = 0;

    /**
     * @brief Print additional algorithm statistics to std::cout.
     */
    virtual void print_additional_statistics() const {}

    /**
     * @brief Runs the encapsulated MDP on the global problem.
     */
    bool solve() override;
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_MDP_SOLVER_H