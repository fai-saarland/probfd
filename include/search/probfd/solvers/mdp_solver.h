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

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
class ProbabilisticTask;

/// This namespace contains the solver plugins for various search algorithms.
namespace solvers {

/**
 * @brief Base interface for MDP solvers.
 */
class MDPSolver : public SolverInterface {
protected:
    const std::shared_ptr<ProbabilisticTask> task;
    const ProbabilisticTaskProxy task_proxy;
    const std::shared_ptr<FDRCostFunction> task_cost_function;
    mutable utils::LogProxy log;

    const std::unique_ptr<TaskStateSpace> task_mdp;
    const std::shared_ptr<FDREvaluator> heuristic;

    ProgressReport progress_;

    const double max_time;

    bool solution_found = true;

public:
    /**
     * @brief Constructs the MDP solver from the given options.
     */
    explicit MDPSolver(const plugins::Options& opts);
    ~MDPSolver();

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
    virtual void solve() override;

    /**
     * @brief Checks if the MDP algorithm found a solution.
     */
    virtual bool found_solution() const override { return solution_found; }

    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace solvers
} // namespace probfd

#endif // __MDP_SOLVER_H__