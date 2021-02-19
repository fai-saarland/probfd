#include "../engines/interval_iteration.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../state_evaluator.h"
#include "mdp_solver.h"

namespace probabilistic {

using IIEngine = interval_iteration::
    IntervalIteration<GlobalState, const ProbabilisticOperator*>;

class IntervalIterationSolver : public MDPSolver {
public:
    explicit IntervalIterationSolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(
              opts.contains("eval")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval")
                  : nullptr)
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "interval_iteration";
    }

    virtual MDPEngineInterface<GlobalState>* create_engine() override
    {
        return engine_factory<IIEngine>(prune_.get(), false);
    }

private:
    std::shared_ptr<GlobalStateEvaluator> prune_;
};

static Plugin<SolverInterface> _plugin(
    "interval_iteration",
    options::parse<SolverInterface, IntervalIterationSolver>);

} // namespace probabilistic
