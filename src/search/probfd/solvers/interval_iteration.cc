#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/interval_iteration.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

namespace probfd::solvers {
namespace {

using namespace algorithms::interval_iteration;
using namespace plugins;

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

        MDPSolver::add_options_to_feature(*this);
    }
};

} // namespace

static FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace probfd::solvers
