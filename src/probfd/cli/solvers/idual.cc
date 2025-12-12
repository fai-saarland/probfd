#include "probfd/cli/solvers/idual.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/idual.h"

#include "downward/lp/lp_solver.h"

#include "downward/operator_id.h"
#include "downward/state.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace language::plugins;

using downward::cli::lp::add_lp_solver_option_to_feature;

namespace {
class IDualSolver : public StatisticalMDPAlgorithmFactory {
    const lp::LPSolverType solver_type_;
    const double fp_epsilon_;

public:
    IDualSolver(lp::LPSolverType lp_solver_type, double fp_epsilon)
        : solver_type_(lp_solver_type)
        , fp_epsilon_(fp_epsilon)
    {
    }

    std::string get_algorithm_name() const override { return "idual"; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask&) override
    {
        using IDualAlgorithm = algorithms::idual::IDual<State, OperatorID>;

        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<IDualAlgorithm>(solver_type_, fp_epsilon_));
    }
};

class IDualSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          lp::LPSolverType,
          double)> {
public:
    IDualSolverFeature()
        : InternalFunctionDefinition("idual", &IDualSolverFeature::func)
    {
        document_title("i-dual");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);

        const auto n2 = add_lp_solver_option_to_feature(*this, n);

        make_optional_argument_with_default(
            n + n2,
            "fp_epsilon",
            "0.0001",
            "The tolerance to use when checking for non-zero values in an LP "
            "solution.");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        lp::LPSolverType lp_solver_type,
        double fp_epsilon)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<IDualSolver>(
                lp_solver_type,
                fp_epsilon),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_idual_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definition<IDualSolverFeature>();
}

} // namespace probfd::cli::solvers
