#include "probfd/solvers/mdp_solver.h"

#include "probfd/cost_model.h"

#include "probfd/engines/ta_topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

std::shared_ptr<TaskEvaluator> get_evaluator(const options::Options& opts)
{
    if (opts.contains("eval")) {
        return opts.get<std::shared_ptr<TaskEvaluator>>("eval");
    }

    return std::make_shared<heuristics::ConstantEvaluator<State>>(
        g_cost_model->optimal_value_bound().upper);
}

class TATopologicalVISolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> prune_;

public:
    explicit TATopologicalVISolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(get_evaluator(opts))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "ta_topological_value_iteration";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        using TVIEngine = engines::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return engine_factory<TVIEngine>(prune_.get());
    }
};

static Plugin<SolverInterface> _plugin(
    "ta_topological_value_iteration",
    options::parse<SolverInterface, TATopologicalVISolver>);

} // namespace
} // namespace solvers
} // namespace probfd
