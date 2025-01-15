#include "downward/cli/plugins/plugin.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/idual.h"

#include "downward/lp/lp_solver.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

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

class IDualSolver : public MDPSolver {
    lp::LPSolverType solver_type_;

public:
    template <typename... Args>
    IDualSolver(lp::LPSolverType lp_solver_type, Args&&... args)
        : MDPSolver(std::forward<Args>(args)...)
        , solver_type_(lp_solver_type)
    {
    }

    std::string get_algorithm_name() const override { return "idual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        using IDualAlgorithm = algorithms::idual::IDual<State, OperatorID>;

        return std::make_unique<IDualAlgorithm>(solver_type_);
    }
};

class IDualSolverFeature : public TypedFeature<TaskSolverFactory, IDualSolver> {
public:
    IDualSolverFeature()
        : TypedFeature<TaskSolverFactory, IDualSolver>("idual")
    {
        document_title("i-dual");

        add_lp_solver_option_to_feature(*this);

        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<IDualSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<IDualSolver>(
            get_lp_solver_arguments_from_options(options),
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<IDualSolverFeature> _plugin;

} // namespace
