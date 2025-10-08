#include "probfd/cli/solvers/idual.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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

using namespace downward::cli::plugins;

using downward::cli::lp::add_lp_solver_option_to_feature;
using downward::cli::lp::get_lp_solver_arguments_from_options;

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

class IDualSolverFeature : public SharedTypedFeature<TaskSolverFactory> {
public:
    IDualSolverFeature()
        : SharedTypedFeature("idual")
    {
        document_title("i-dual");

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
            make_shared_from_arg_tuples<IDualSolver>(
                get_lp_solver_arguments_from_options(options),
                options.get<double>("fp_epsilon")),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_idual_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<IDualSolverFeature>();
}

} // namespace probfd::cli::solvers
