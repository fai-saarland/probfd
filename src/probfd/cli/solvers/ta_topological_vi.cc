#include "probfd/cli/solvers/ta_topological_vi.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace language::parser;

namespace {
class TATopologicalVISolver : public StatisticalMDPAlgorithmFactory {
    const value_t convergence_epsilon_;

public:
    explicit TATopologicalVISolver(value_t convergence_epsilon)
        : convergence_epsilon_(convergence_epsilon)
    {
    }

    std::string get_algorithm_name() const override
    {
        return "ta_topological_value_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask&) override
    {
        using TVIAlgorithm = algorithms::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<TVIAlgorithm>(convergence_epsilon_));
    }
};

std::shared_ptr<TaskSolverFactory> create_tatvi(
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    value_t report_epsilon,
    bool report_enabled,
    utils::Verbosity verbosity,
    value_t convergence_epsilon)
{
    return make_shared_from_arg_tuples<MDPSolver>(
        make_shared_from_arg_tuples<TATopologicalVISolver>(convergence_epsilon),
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

void add_ta_topological_value_iteration_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<create_tatvi>(
        nspace,
        "ta_topological_value_iteration");

    f.document_title("Trap-Aware Topological Value Iteration");
    const auto n = add_base_solver_options_except_algorithm_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "convergence_epsilon",
        "10e-4",
        "The tolerance for convergence checks.");
}

} // namespace probfd::cli::solvers
