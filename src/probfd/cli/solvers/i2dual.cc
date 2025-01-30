#include "downward/cli/plugins/plugin.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/i2dual.h"

#include <memory>
#include <string>

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

using downward::cli::lp::add_lp_solver_option_to_feature;
using downward::cli::lp::get_lp_solver_arguments_from_options;

namespace {

class I2DualSolver : public MDPSolver {
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;
    double fp_epsilon_;

public:
    template <typename... Args>
    I2DualSolver(
        bool disable_hpom,
        bool incremental_updates,
        lp::LPSolverType lp_solver,
        double fp_epsilon,
        Args&&... args)
        : MDPSolver(std::forward<Args>(args)...)
        , hpom_enabled_(!disable_hpom)
        , incremental_hpom_updates_(incremental_updates)
        , solver_type_(lp_solver)
        , fp_epsilon_(fp_epsilon)
    {
    }

    std::string get_algorithm_name() const override { return "i2dual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return std::make_unique<algorithms::i2dual::I2Dual>(
            task,
            task_cost_function,
            hpom_enabled_,
            incremental_hpom_updates_,
            solver_type_,
            fp_epsilon_);
    }
};

class I2DualSolverFeature
    : public TypedFeature<TaskSolverFactory, I2DualSolver> {
public:
    I2DualSolverFeature()
        : TypedFeature<TaskSolverFactory, I2DualSolver>("i2dual")
    {
        document_title("i^2-dual");

        add_option<bool>("disable_hpom", "", "false");
        add_option<bool>("incremental_updates", "", "true");

        add_lp_solver_option_to_feature(*this);

        add_option<double>(
            "fp_epsilon",
            "The tolerance to use when checking for non-zero values in an LP "
            "solution.",
            "0.0001");

        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<I2DualSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<I2DualSolver>(
            options.get<bool>("disable_hpom"),
            options.get<bool>("incremental_updates"),
            get_lp_solver_arguments_from_options(options),
            options.get<double>("fp_epsilon"),
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<I2DualSolverFeature> _plugin;

} // namespace
