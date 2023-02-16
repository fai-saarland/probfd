#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/interval_iteration.h"

#include "probfd/engine_interfaces/state_evaluator.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

using IIEngine =
    engines::interval_iteration::IntervalIteration<State, OperatorID>;

class IntervalIterationSolver : public MDPSolver {
public:
    explicit IntervalIterationSolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(
              opts.contains("eval")
                  ? opts.get<std::shared_ptr<TaskStateEvaluator>>("eval")
                  : nullptr)
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
        return "interval_iteration";
    }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        return engine_factory<IIEngine>(prune_.get(), false, false);
    }

private:
    std::shared_ptr<TaskStateEvaluator> prune_;
};

static Plugin<SolverInterface> _plugin(
    "interval_iteration",
    options::parse<SolverInterface, IntervalIterationSolver>);

} // namespace
} // namespace solvers
} // namespace probfd
