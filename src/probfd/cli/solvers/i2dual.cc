#include "probfd/cli/solvers/i2dual.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

class I2DualSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          bool,
          bool,
          lp::LPSolverType,
          double)> {
public:
    I2DualSolverFeature()
        : InternalFunctionDefinition("i2dual", &I2DualSolverFeature::func)
    {
        document_title("i^2-dual");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);

        make_optional_argument_with_default(n, "disable_hpom", "false");
        make_optional_argument_with_default(
            n + 1,
            "incremental_updates",
            "true");

        const auto n2 = add_lp_solver_option_to_feature(*this, n + 2);

        make_optional_argument_with_default(
            n + n2 + 2,
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
        downward::utils::Verbosity verbosity,
        bool disable_hpom,
        bool incremental_updates,
        lp::LPSolverType lp_solver,
        double fp_epsilon)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<I2DualSolver>(
                disable_hpom,
                incremental_updates,
                lp_solver,
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

void add_i2dual_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definition<I2DualSolverFeature>();
}

} // namespace probfd::cli::solvers
