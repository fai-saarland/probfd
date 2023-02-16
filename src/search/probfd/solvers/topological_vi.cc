#include "probfd/solvers/mdp_solver.h"

#include "probfd/cost_model.h"

#include "probfd/engines/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/engine_interfaces/state_evaluator.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using TVIEngine =
    engines::topological_vi::TopologicalValueIteration<State, OperatorID>;

std::shared_ptr<TaskStateEvaluator> get_evaluator(const options::Options& opts)
{
    if (opts.contains("eval")) {
        return opts.get<std::shared_ptr<TaskStateEvaluator>>("eval");
    }

    return std::make_shared<heuristics::ConstantEvaluator<State>>(
        g_cost_model->optimal_value_bound().upper);
}

class TopologicalVISolver : public MDPSolver {
public:
    explicit TopologicalVISolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(get_evaluator(opts))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<TaskStateEvaluator>>(
            "eval",
            "",
            options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "topological_value_iteration";
    }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        return engine_factory<TVIEngine>(prune_.get(), false);
    }

private:
    std::shared_ptr<TaskStateEvaluator> prune_;
};

static Plugin<SolverInterface> _plugin(
    "topological_value_iteration",
    options::parse<SolverInterface, TopologicalVISolver>);

} // namespace
} // namespace solvers
} // namespace probfd
