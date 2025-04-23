#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/interval_iteration.h"

#include "downward/operator_id.h"
#include "downward/state.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::interval_iteration;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

class IntervalIterationSolver : public StatisticalMDPAlgorithmFactory {
    const value_t convergence_epsilon_;

public:
    explicit IntervalIterationSolver(value_t convergence_epsilon)
        : convergence_epsilon_(convergence_epsilon)
    {
    }

    std::string get_algorithm_name() const override
    {
        return "interval_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const std::shared_ptr<ProbabilisticTask>&) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<IntervalIteration<State, OperatorID>>(
                convergence_epsilon_,
                false,
                false));
    }
};

class IntervalIterationSolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    IntervalIterationSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("interval_iteration")
    {
        document_title("Interval Iteration");

        add_base_solver_options_except_algorithm_to_feature(*this);

        add_option<probfd::value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<IntervalIterationSolver>(
                options.get<value_t>("convergence_epsilon")),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace
