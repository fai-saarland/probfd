#ifndef PROBFD_SOLVERS_MDP_SOLVER_H
#define PROBFD_SOLVERS_MDP_SOLVER_H

#include "probfd/solver_interface.h"

#include "probfd/algorithms/fdr_types.h"

#include "probfd/cost_function.h"
#include "probfd/evaluator.h"
#include "probfd/fdr_types.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/progress_report.h"
#include "probfd/task_proxy.h"
#include "probfd/task_state_space.h"

#include "downward/utils/logging.h"

#include "downward/state_registry.h"

#include <memory>
#include <string>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
class TaskStateSpace;

/// This namespace contains the solver plugins for various search algorithms.
namespace solvers {

/**
 * @brief Base interface for MDP solvers.
 */
class MDPSolver : public SolverInterface {
protected:
    const std::shared_ptr<ProbabilisticTask> task;
    ProbabilisticTaskProxy task_proxy;
    const std::shared_ptr<FDRCostFunction> task_cost_function;
    mutable utils::LogProxy log;

    std::unique_ptr<TaskStateSpace> task_mdp;
    std::shared_ptr<FDREvaluator> heuristic;

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
     * @brief Factory method that constructs an new MDP algorithm from the
     * given arguments.
     *
     * @tparam Algorithm - The algorithm type to construct.
     */
    template <typename Algorithm, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> algorithm_factory(Args&&... args)
    {
        return std::make_unique<Algorithm>(std::forward<Args>(args)...);
    }

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