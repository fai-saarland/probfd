#include "probfd_plugins/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/i2dual.h"

#include "downward/plugins/plugin.h"

#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd_plugins::solvers;

namespace {

class I2DualSolver : public MDPSolver {
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;

public:
    I2DualSolver(
        bool disable_hpom,
        bool incremental_updates,
        lp::LPSolverType lp_solver,
        utils::Verbosity verbosity,
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
              std::move(path_dependent_evaluators),
              cache,
              eval,
              report_epsilon,
              report_enabled,
              max_time,
              std::move(policy_filename),
              print_fact_names)
        , hpom_enabled_(!disable_hpom)
        , incremental_hpom_updates_(incremental_updates)
        , solver_type_(lp_solver)
    {
    }

    std::string get_algorithm_name() const override { return "i2dual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return std::make_unique<algorithms::i2dual::I2Dual>(
            this->task_,
            this->task_cost_function_,
            hpom_enabled_,
            incremental_hpom_updates_,
            solver_type_);
    }
};

class I2DualSolverFeature : public TypedFeature<SolverInterface, I2DualSolver> {
public:
    I2DualSolverFeature()
        : TypedFeature<SolverInterface, I2DualSolver>("i2dual")
    {
        document_title("i^2-dual");

        add_option<bool>("disable_hpom", "", "false");
        add_option<bool>("incremental_updates", "", "true");

        lp::add_lp_solver_option_to_feature(*this);

        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<I2DualSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<I2DualSolver>(
            options.get<bool>("disable_hpom"),
            options.get<bool>("incremental_updates"),
            lp::get_lp_solver_arguments_from_options(options),
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<I2DualSolverFeature> _plugin;

} // namespace
