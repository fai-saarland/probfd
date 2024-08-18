#include "probfd_plugins/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/idual.h"

#include "downward/lp/lp_solver.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd_plugins::solvers;

namespace {

class IDualSolver : public MDPSolver {
    lp::LPSolverType solver_type_;

public:
    IDualSolver(
        lp::LPSolverType lp_solver_type,
        utils::Verbosity verbosity,
        const std::shared_ptr<TaskCostFunctionFactory>& costs,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
        bool cache,
        const std::shared_ptr<TaskEvaluatorFactory>& eval,
        std::optional<value_t> report_epsilon,
        bool report_enabled,
        double max_time,
        std::string policy_filename,
        bool print_fact_names)
        : MDPSolver(
              verbosity,
              costs,
              std::move(path_dependent_evaluators),
              cache,
              eval,
              report_epsilon,
              report_enabled,
              max_time,
              std::move(policy_filename),
              print_fact_names)
        , solver_type_(lp_solver_type)
    {
    }

    std::string get_algorithm_name() const override { return "idual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using IDualAlgorithm = algorithms::idual::IDual<State, OperatorID>;

        return std::make_unique<IDualAlgorithm>(solver_type_);
    }
};

class IDualSolverFeature : public TypedFeature<SolverInterface, IDualSolver> {
public:
    IDualSolverFeature()
        : TypedFeature<SolverInterface, IDualSolver>("idual")
    {
        document_title("i-dual");

        lp::add_lp_solver_option_to_feature(*this);

        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<IDualSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<IDualSolver>(
            lp::get_lp_solver_arguments_from_options(options),
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<IDualSolverFeature> _plugin;

} // namespace
