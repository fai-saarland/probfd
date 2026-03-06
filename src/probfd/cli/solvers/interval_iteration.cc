#include "probfd/cli/solvers/interval_iteration.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/interval_iteration.h"

#include "downward/operator_id.h"
#include "downward/state.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::interval_iteration;

using namespace probfd::cli::solvers;

using namespace language::parser;

namespace {
class IntervalIterationSolver : public StatisticalMDPAlgorithmFactory {
    const value_t convergence_epsilon_;

public:
    explicit IntervalIterationSolver(value_t convergence_epsilon)
        : convergence_epsilon_(convergence_epsilon)
    {
    }

    std::string get_algorithm_name() const override
    {
        return "interval_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask&) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<IntervalIteration<State, OperatorID>>(
                convergence_epsilon_,
                false,
                false));
    }
};

std::shared_ptr<TaskSolverFactory> create_ii(
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    value_t report_epsilon,
    bool report_enabled,
    Verbosity verbosity,
    value_t convergence_epsilon)
{
    return make_shared_from_arg_tuples<MDPSolver>(
        make_shared_from_arg_tuples<IntervalIterationSolver>(
            convergence_epsilon),
        std::move(task_state_space_factory),
        std::move(heuristic_factory),
        std::move(policy_filename),
        print_fact_names,
        report_epsilon,
        report_enabled,
        verbosity);
}

} // namespace

namespace probfd::cli::solvers {

void add_interval_iteration_solver_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f =
        insert_function_definition<create_ii>(nspace, "interval_iteration");

    f.document_title("Interval Iteration");

    const auto n = add_base_solver_options_except_algorithm_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "convergence_epsilon",
        "10e-4",
        "The tolerance for convergence checks.");
}

} // namespace probfd::cli::solvers
