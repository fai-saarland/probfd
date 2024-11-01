#include "probfd_plugins/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/interval_iteration.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::interval_iteration;

using namespace probfd_plugins::solvers;

namespace {

class IntervalIterationSolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "interval_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return std::make_unique<IntervalIteration<State, OperatorID>>(
            false,
            false);
    }
};

class IntervalIterationSolverFeature
    : public TypedFeature<SolverInterface, IntervalIterationSolver> {
public:
    IntervalIterationSolverFeature()
        : TypedFeature<SolverInterface, IntervalIterationSolver>(
              "interval_iteration")
    {
        document_title("Interval Iteration");

        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<IntervalIterationSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<IntervalIterationSolver>(
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace
