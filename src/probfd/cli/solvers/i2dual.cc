#include "probfd/cli/solvers/i2dual.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/i2dual.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

using downward::cli::lp::add_lp_solver_option_to_feature;
using downward::cli::lp::get_lp_solver_arguments_from_options;

namespace {
class I2DualSolver : public StatisticalMDPAlgorithmFactory {
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;
    double fp_epsilon_;

public:
    I2DualSolver(
        bool disable_hpom,
        bool incremental_updates,
        lp::LPSolverType lp_solver,
        double fp_epsilon)
        : hpom_enabled_(!disable_hpom)
        , incremental_hpom_updates_(incremental_updates)
        , solver_type_(lp_solver)
        , fp_epsilon_(fp_epsilon)
    {
    }

    std::string get_algorithm_name() const override { return "i2dual"; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<algorithms::i2dual::I2Dual>(
                task,
                hpom_enabled_,
                incremental_hpom_updates_,
                solver_type_,
                fp_epsilon_));
    }
};

class I2DualSolverFeature : public TypedFeature<TaskSolverFactory> {
public:
    I2DualSolverFeature()
        : TypedFeature("i2dual")
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

        add_base_solver_options_except_algorithm_to_feature(*this);
    }

protected:
    std::shared_ptr<TaskSolverFactory>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<I2DualSolver>(
                options.get<bool>("disable_hpom"),
                options.get<bool>("incremental_updates"),
                get_lp_solver_arguments_from_options(options),
                options.get<double>("fp_epsilon")),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_i2dual_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<I2DualSolverFeature>();
}

} // namespace probfd::cli::solvers
